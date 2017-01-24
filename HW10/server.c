#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

void serv_client(int fd, struct sockaddr_in *sin){
	//FILE *commandfd = popen("ps aux | wc -l", "r");
	//fgets(output, 10, commandfd);
	//printf("Number:%s\n", output);
	printf("Connected from %s: %d\n", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
	while(1){
		//popen中用了pipe+fork+exec的编程模型 产生一个child process专门用来执行uptime命令 
		FILE *commandfd = popen("ps aux | wc -l", "r");
		char message[] = "Current running processes number: ";
		char output[10];
		fgets(output, 10, commandfd);
		strcat(message, output);
		int len = strlen(message);
		if(send(fd, message, len, 0) < 0){
			printf("send error\n");
			exit(-1);
		}
		sleep(1);
	}
	printf("Disconnected from %s: %d\n", inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
	return;

}

int main(int argc, char* argv[]){
	pid_t pid;
	int serverSockfd, clientSockfd, serverPort, val;

	// sin is server in
	struct sockaddr_in serverIn, clientIn;
	serverPort = atoi(argv[1]);
	//printf("serverPort:%d\n", serverPort);
	if((serverSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		printf("Socket error\n");
		return(-1);
	}
	val = 1;

	//ignore child termination
	signal(SIGCHLD, SIG_IGN);
	// the option is a generic socket-level option, then level is set to SOL_SOCKET
	// SO_REUSEADDR is to reuse addresses in bind if *val is nonzero
	if(setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0){
		printf("Setsockopt error\n");
		return(-1);
	}
		
	// initial
	bzero(&serverIn, sizeof(serverIn));
	serverIn.sin_family = AF_INET;
	//serverIn.sin_addr.s_addr = INADDR_ANY;
	serverIn.sin_port = htons(serverPort);
	if(bind(serverSockfd, (struct sockaddr *)&serverIn, sizeof(serverIn)) < 0){
		printf("Bind error\n");
		return(-1);
	}
	
	if(listen(serverSockfd, SOMAXCONN) < 0){
		printf("listen error\n");
		return(-1);
	}

	// server start to work
	while(1){
		val = sizeof(clientIn);
		bzero(&clientIn, sizeof(clientIn));

		// if no connect requests are pending, accept will block until one arrives
		if((clientSockfd = accept(serverSockfd,  (struct sockaddr*)&clientIn, &val)) < 0){
			printf("accept error\n");
			return(-1);
		}
		if((pid = fork()) <0){
			printf("fork error\n");
			return(-1);
		}
		else if(pid == 0){	// child
			close(serverSockfd);
			serv_client(clientSockfd, &clientIn);
			exit(0);
		}
		// parent
		//这次处理client的请求完毕 关闭与client连接的socket
		close(clientSockfd);
	}
	return 0;
}
