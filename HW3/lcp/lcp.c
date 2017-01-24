#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
int main(int argc, char *argv[]){
	// input(origin)
	int fd = open(argv[1], O_RDONLY);
	// output(copied file)
	int fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);

	struct stat status, status2;
	stat(argv[1], &status);
	stat(argv[2], &status2);
	status2.st_uid = status.st_uid;
	status2.st_gid = status.st_gid;
	int i, cnt=0;
	char buffer;
	while(read(fd, &buffer, 1) != 0){
		/*
		if(buffer == '\0')cnt++;
		else {
			if(cnt == 0){
				write(fd2, &buffer, 1);
			}
			else{
				if(lseek(fd2, cnt, SEEK_CUR) == -1)printf("lseek error\n");
				else{
					cnt = 0;
					write(fd2, &buffer, 1);
				}
			}
		}
		*/
		write(fd2, &buffer, 1);
	}
	close(fd);
	close(fd2);
	
	return 0;
}
