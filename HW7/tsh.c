#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#define MAXJOBS 30
#define debug	0
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

struct jobStruct{
	pid_t jobPID;			// job's process ID
	char jobCmd[100]; 		// job's command
	int isBackgroundJob;		// fg or bg
	int state;			// 0 is running, 1 is stop, 2 is done		
};

struct jobStruct jobList[MAXJOBS];					// job's order index = list index + 1
static pid_t shellPID, shellPGID;					//Shell initial process group ID
int jobCount;

void deleteJobFromList(pid_t pid);
int addJobToList(pid_t pid, char *cmdLine, int isBackgroundJob);
int findJobIndex(pid_t pid);

void sigquit_handler(int signo){
	printf("Terminate! Signal:SIGQUIT\n");
	exit(1);
}
void sigchld_handler(int signo){
	//signal(SIGCHLD, sigchld_handler);                               // Signal child
	pid_t pid;
	int stat;
	// call waitpid(), wait child process to return(if no return, it will not block)
	while((pid = waitpid(-1 ,&stat,WNOHANG | WUNTRACED)) > 0){// reap child with no blocking, if not reaped termination status, return 0
		// if the process is foreground, set back the control terminal
		//printf("CHILD\n");
		if(tcgetpgrp(STDIN_FILENO) != shellPGID) tcsetpgrp(STDIN_FILENO, shellPGID);
		if(WIFEXITED(stat)){
			if(debug)printf("Normal exit\n");
			if(debug)printf("pid:%d, Normal exit\n", pid);
			deleteJobFromList(pid);
		}
		else if(WIFSIGNALED(stat)){
			
			printf("Abnormal exit\n");
			if(debug)printf("Signal:%d\n", WTERMSIG(stat));
			deleteJobFromList(pid);
		}
		else if(WIFSTOPPED(stat)){
			if(debug)printf("Stop\n");
			if(debug)printf("Signal:%d\n", WSTOPSIG(stat));	
		}
	}
	if(tcgetpgrp(STDIN_FILENO) != shellPGID) tcsetpgrp(STDIN_FILENO, shellPGID);
	if(debug)printf("Now pgid:%d, ct:%d\n", getpgrp(), tcgetpgrp(STDIN_FILENO));
	if(debug)printf("signo:%d\n", signo);
	return;
}

int main(){
	int i;
	jobCount = 1;
	signal(SIGINT, SIG_IGN);					// Signal interrupt
	signal(SIGTSTP, SIG_IGN);					// Signal Stop(Stop typed at terminal)
	signal(SIGQUIT, sigquit_handler);				// Signal quit(Quit from keyboard)
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGCHLD, sigchld_handler);                             		// Signal child
	
	/*Initial job list*/
	for(i = 0; i < MAXJOBS; i++){
		jobList[i].jobPID = 0;
		jobList[i].jobCmd[0] = '\0';
		jobList[i].isBackgroundJob = 0;
	}

	// Make my shell its process group leader
	shellPID = getpid();
	//setpgid(getpid(), getpid());

	// Get terminal control from the shell that started my shell
	if(tcgetpgrp(STDIN_FILENO) != shellPGID)tcsetpgrp(STDIN_FILENO, shellPID);
	shellPGID = tcgetpgrp(STDIN_FILENO);					//Shell initial process group ID

	for(;;){
		printf("tsh$:");				// 1. Prints a prompt
		if(debug)printf("Now PID:%d, , Now PGID:%d, shellPGID:%d, Now Control terminal PGID:%d\n", getpid(), getpgrp(), shellPGID, tcgetpgrp(STDIN_FILENO));
		char cmdLine[100];
		char originCmdLine[100];
		char *exeName = NULL;
		char *optionArg = NULL;
		char *argv[100];
		int isBackground = 0;
		int argCount = 0;
		int status;
		pid_t pid;
		
		memset(cmdLine, '\0', sizeof(cmdLine));
		memset(originCmdLine, '\0', sizeof(originCmdLine));
		memset(argv, '\0', sizeof(argv));		
		new_fgets(cmdLine, sizeof(cmdLine), stdin);	// 2. Reads a command line (ordinary cmd, consist an executable program name and an argument)
		if(strcmp(cmdLine, "") == 0){				// argv[0] == NULL
			continue;
		}

		// copy cmdLine to originCmdLine for a backup
		strcpy(originCmdLine, cmdLine);
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
		else if(strcmp(exeName, "jobs") == 0){
			for(i = 0; i < MAXJOBS; i++){
				if(jobList[i].jobPID == 0)continue;
				else{
					if(jobList[i].isBackgroundJob == 1)printf("[%d] %d %s\n", findJobIndex(jobList[i].jobPID)+1, jobList[i].jobPID, jobList[i].jobCmd);
				}
				
			}
		}
		else if(strcmp(exeName, "fg") == 0){
			if(argv[1][0] - '0' <= 0){
				printf("%s: %s: no such job\n", argv[0], argv[1]);
				continue;
			}
			else{
				int fgJobIndex = argv[1][0] - '0' - 1;
				// check handling
				if(jobList[fgJobIndex].jobPID != 0){
					jobList[fgJobIndex].isBackgroundJob = 0;
					tcsetpgrp(STDIN_FILENO, jobList[fgJobIndex].jobPID);
					// parent wait(shell wait)
					//while(tcgetpgrp(STDIN_FILENO) == getpgid(jobList[fgJobIndex].jobPID));
					if(waitpid(jobList[fgJobIndex].jobPID, &status, 0) < jobList[fgJobIndex].jobPID){
						printf("waitpid error\n");
						exit(1);
					}
					deleteJobFromList(jobList[fgJobIndex].jobPID);
				}
			}
		}
		else{

			if((pid = fork()) < 0){					// error 
				printf("fork error\n");
				exit(1);
			}
			else if(pid == 0){						// child
				if(debug)printf("--Child PID:%d--\n", getpid());			
				// set child process group id, make yourself process group leader
				setpgid(0, 0);	
			//	tcsetpgrp(STDIN_FILENO, getpgrp());
				
				signal(SIGINT, SIG_DFL);                   	// Signal interrupt
               			signal(SIGTSTP, SIG_DFL);                   	// Signal Stop(Stop typed at terminal)
                	        signal(SIGQUIT, SIG_DFL);      		// Signal 
				
				if((execvp(exeName, argv)) == -1){		// error checking: Any command not found in one of the directories on $PATH
					printf("%s: command not found\n", exeName);
					exit(1);
				}
			}
			else{
				if(debug)printf("--Parent PID:%d--\n", getpid());			

				// Make child its own process group leader, to avoid race condition
				setpgid(pid, pid);
				
				// Now we have two processes, parent and child
				// The parent, which ignores the signal, the child will set signal handler default
				// tansfer controlling terminal
				if(isBackground == 1){
					addJobToList(pid, originCmdLine, 1);
					printf("[%d] %d %s\n", findJobIndex(pid)+1, pid, originCmdLine);
				}
				else{
					tcsetpgrp(STDIN_FILENO, getpgid(pid));				
					//addJobToList(pid, originCmdLine, 0);
				}
				
				if(debug)printf("Child PID:%d, Parent wait:%d, child group leader:%d, shell:%d\n", pid, tcgetpgrp(STDIN_FILENO), getpgid(pid), shellPGID);				
				//parent wait
				while(tcgetpgrp(STDIN_FILENO) == getpgid(pid));
					
				if(debug)printf("Parent wait:%d, child group leader:%d, shell:%d\n", tcgetpgrp(STDIN_FILENO), getpgid(pid), shellPGID);				
				if(debug)printf("Child terminate, Parent:%d\n", tcgetpgrp(STDIN_FILENO));				
			}
		}
	}
	return 0;
}
void deleteJobFromList(pid_t pid){
	int i;
	for(i = MAXJOBS - 1; i >= 0; i--){
		if(jobList[i].jobPID == pid){
			jobList[i].jobPID = 0;
		        //strcpy(jobList[i].jobCmd, '\0');
			memset(jobList[i].jobCmd, '\0', sizeof(jobList[i].jobCmd));
		        jobList[i].isBackgroundJob = 0;
			break;	
		}	
	} 
}
int addJobToList(pid_t pid, char *cmdLine, int isBackgroundJob){
	if(pid < 1) return 0; // error(pid == 0 means child)
	int i;
	int assignJobID = 0;
	for(i = MAXJOBS - 1; i >= 0 ; i--){
		if(jobList[i].jobPID != 0){
			assignJobID = i+1;
			break;
		}
		if(i == 0){ 
			assignJobID = 0;
			break;
		}
	}
	jobList[assignJobID].jobPID = pid;
	strcpy(jobList[assignJobID].jobCmd, cmdLine);
	jobList[assignJobID].isBackgroundJob = isBackgroundJob;
	return 0;

}
int findJobIndex(pid_t pid){
	int i;
	for(i = 0; i < MAXJOBS; i++){
		if(jobList[i].jobPID == pid){
			return i;
		}
	}
}
