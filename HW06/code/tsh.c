#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
char* new_fgets(char* buf, int num, FILE* fp)		// fgets improved, ref: http://hsian-studio.blogspot.tw/2008/09/fgets.html
{
	char* find = 0;
	if (!fgets(buf, num, fp)){
		return NULL;
	}
	if ((find = strrchr(buf, '\n'))){
		*find = '\0';
	}
	return buf;
}

int main(){
	
	for(;;){
		int jobCount = 1;
		int status;
		char cmdLine[100];
		char *exeName = NULL;
		char *optionArg = NULL;
		char *argv[100];
		int isBackground = 0;
		int argCount = 0;
		//pid_t pid1;
		//pid_t pid2;
		pid_t pid;

		printf("tsh$:");								// 1. Prints a prompt
		new_fgets(cmdLine, sizeof(cmdLine), stdin);	// 2. Reads a command line (ordinary cmd, consist an executable program name and an argument)
		if(strcmp(cmdLine, "") == 0){
			continue;
		}
		exeName = strtok(cmdLine, " ");				// 3. Parse the cmd into tokens, store in argv
		argv[argCount++] = exeName;
		optionArg = strtok(NULL, " ");
		while(optionArg != NULL){
			argv[argCount++] = optionArg;
			optionArg = strtok(NULL, " ");
		}
		
													// Background mode or not
		if(strchr(argv[argCount-1], '&')){
			isBackground = 1;
			argv[argCount-1] = 0;
			argCount--;
		}
		else{
			argv[argCount] = 0;
		}
												// The last argument of argv is NULL
												// check the last two argument is & or not
												// 4. Fork a child:loads and executes the command; // child process
		if(strcmp(exeName, "cd") == 0){			// error checking: cd error msg
			if(argv[1] == NULL){
				chdir(getenv("HOME"));
			}
			else{
				if(chdir(argv[1]) == -1){
					if(errno == ENOENT){
						printf("cd: %s: No such file or directory\n", argv[1]);
						exit(1);
					}
					else if(errno == ENOTDIR){
						printf("cd: %s: Not a directory\n", argv[1]);
						exit(1);
					}
				}
			}
		}
		else{
			
			if((pid = fork()) < 0){					// error 
				printf("fork error\n");
				exit(1);
			}
			else if(pid == 0){						// child
				if(isBackground == 1){
					if((pid = fork()) < 0){			// prevent zombie process
						printf("fork error\n");
					}
					else if(pid > 0){
						exit(0);
					}
					
				}

				//jobCount++;
				if((execvp(exeName, argv)) == -1){	// error checking: Any command not found in one of the directories on $PATH
					printf("%s: command not found\n", exeName);
					exit(1);
				}
				exit(0);
			}
			else{
			/*
				if(isBackground == 1){						// parent
					printf("[%d] %d\n", jobCount, pid);
			//		continue;
				}
			*/
				if(waitpid(pid, NULL, 0) != pid){
					printf("waitpid error\n");
					exit(1);
				}
			}
			
			/*
			if(pid1 = fork()){
				waitpid(pid1, &status, NULL);
			}
			else if(!pid1){
				if(pid2 = fork()){
					exit(0);
				}
				else if(!pid2){
					execvp(exeName, argv);
				}
			}
			*/
		}
	}
	return 0;
}
