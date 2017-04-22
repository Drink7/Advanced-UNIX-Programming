#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

int main(int argc, char* argv[]){

	char buffer[2048];
	int serverPort = atoi(argv[2]);
	//printf("IP:%s, port:%d\n", serverIP, serverPort);
	int serverSockfd, val, len;

        // sin is server in
        struct sockaddr_in serverIn;
	struct in_addr formatAddr;
        //printf("serverPort:%d\n", serverPort);
        if((serverSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
                printf("Socket error\n");
                return(-1);
        }
        val = 1;
        //signal(SIGCHLD, SIG_IGN);
        // the option is a generic socket-level option, then level is set to SOL_SOCKET
        // SO_REUSEADDR is to reuse addresses in bind if *val is nonzero
        if(setsockopt(serverSockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0){
                printf("Setsockopt error\n");
                return(-1);
        }
	bzero(&serverIn, sizeof(serverIn));

	// Convert text to formatted address
	inet_pton(AF_INET, argv[1], &formatAddr);
	serverIn.sin_family = AF_INET;
        serverIn.sin_port = htons(serverPort);
	serverIn.sin_addr.s_addr = formatAddr.s_addr;
	//printf("IPv4: 0x%08x\n", htonl(formatAddr.s_addr));
	if(connect(serverSockfd, (struct sockaddr*) &serverIn, sizeof(serverIn)) < 0){
		printf("connect error\n");
		return -1;
	}
	while((len = recv(serverSockfd, buffer, sizeof(buffer), 0)) > 0){
		printf("%s", buffer);
	}
	return 0;
}
