#include "hw2.h"
#include <unistd.h>
#define OPEN_MAX 1024

int do_mydup2(int oldfd, int newfd);
int mydup2(int oldfd, int newfd){
	//Your implementation here
	
	int tmp;

	// check fd is in the range or not
	if(oldfd < 0 || newfd < 0){
		return -1;
	}
	
	if(newfd >= OPEN_MAX || oldfd >= OPEN_MAX){
		return -1;
	}

	// check fd active or not (null or not)
	tmp = dup(oldfd);
	if(tmp == -1){
		return -1;
	}
	else{
		close(tmp);
	}

	// check oldfd == newfd or not
	if(oldfd == newfd){
		return oldfd;
	}

	// start do dup2 and return 
	close(newfd);
	return do_mydup2(oldfd, newfd);
}

int do_mydup2(int oldfd, int newfd){
	int i = 0;
	int count = 0;
	int dup_array[OPEN_MAX];
	int isNewfd = -1;
	
	// Brute force to get the newfd
	// we have check the newfd value so the newfd will not exceed 1024
	//	dup -> 2 dup -> 3 dup ......
	// store the file descriptors we generate now, we need to recover them after we find out our newfd
	for(i = 0; i < newfd; i++){
		dup_array[i] = dup(oldfd);
		
		// dup(oldfd) is inactive 
		if(dup_array[i] == -1){
			break;
		}
		if(dup_array[i] == newfd){
			
			// find the index
			isNewfd = i;
			break;
		}
	}
	
	// check find the index or not
	// here if the isNewfd is -1 means the index breaks from loop represent error
	// so we need to recover other used fd
	if(isNewfd == -1){
		for(i = 0; i < newfd; i++){
			if(dup_array[i] == -1)break;
			else close(dup_array[i]);
		}
		return -1;
		
	}
	else{
		for(i = 0; i < isNewfd; i++){
			close(dup_array[i]);
		}
	}
	
	return newfd;
}
