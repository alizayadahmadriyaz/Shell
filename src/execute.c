#include "execute.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>


int execute(char *command, char *args[], int bg){
	int cpid=  fork();
	if(cpid == 0){
		signal (SIGINT, SIG_DFL);
	 	signal (SIGQUIT, SIG_DFL);
	 	signal (SIGTSTP, SIG_DFL);
	 	signal (SIGTTIN, SIG_DFL);
	 	signal (SIGTTOU, SIG_DFL);
	 	signal (SIGCHLD, SIG_DFL);

		if(bg) setpgid(0,0);
		if(execvp(command,args)== -1){
			perror(command);
			return -1;
		}
	}
	else{
		if(!bg){
			int status=0;
			waitpid(cpid, &status, WUNTRACED);
			if(WIFEXITED(status)){
				// printf("normal\n");
				return -1;
			}
			else if(WIFSTOPPED(status)){
				// printf("HERE\n");
				return cpid;
			}
		}
		// printf("Howintheworld\n");
		return cpid;
	}
}

