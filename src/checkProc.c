#include "checkProc.h"
#include "mylimits.h"
#include "pinfo.h"
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int pidStack[MAXN];
char processName[MAXN][50];
int pidStackSize = 0;

void kjob(int ind, int signum){
	if(ind > pidStackSize){
		printf("No Such Process\n");
		return;
	}
	int pid = pidStack[ind];
	kill(pid, signum);
	
}

void fg(int ind, int shellpgrp){
	if(ind > pidStackSize){
		printf("No Such Process\n");
		return;
	}
	int pid = pidStack[ind];
	int pgid = getpgid(pid);
	// printf("proc to wake up pid:%d pgid:%d\n",pid,pgid );
	tcsetpgrp(STDIN_FILENO, pgid);
	kill(pid, SIGCONT);
	int status;
	waitpid(pid,&status, WUNTRACED);
	if(WIFSTOPPED(status)){
		fprintf(stderr,"[%d] %d\n",ind+1, pid);	
	}
	tcsetpgrp(STDIN_FILENO, shellpgrp);
}

void bgco(int ind){
	if(ind > pidStackSize){
		printf("No Such Process\n");
		return;
	}
	int pid = pidStack[ind];
	int pgid = getpgid(pid);
	kill(pgid, SIGCONT);
}

void overkill(){
	for(int i=0;i<pidStackSize;i++){
		int pid = pidStack[i];
		kill(pid,SIGKILL);
	}
	pidStackSize = 0;
}


void jobs(){
	for(int i=0;i<pidStackSize;i++){
		// printf("%d\n",pidStack[i]);
		char status[MAXN]="";
		int pid = pidStack[i];
		getStatus(pid,status);
		printf("[%d] %s\t%s\n",i+1, status, processName[i]);
	}
}

void checkProc(){

	for(int i=0;i<pidStackSize;i++){
		// printf("%d\n",pidStack[i]);
		int status = 0;
		if(waitpid(pidStack[i], &status, WNOHANG)){
			if(WIFEXITED(status)){
				int printstate = WEXITSTATUS(status);
				fprintf(stderr,"[%d] Done with Status %d\t", i+1, printstate);
				fprintf(stderr,"%s\n", processName[i]);
				popProc(i);
			}
		}
	}
}

void popProc(int i){
	for(int j=i;j<pidStackSize;j++){
		pidStack[j] = pidStack[j+1];
		strcpy(processName[j],processName[j+1]);
	}
	pidStackSize -= 1;
}

void addProc(int cpid,char *syscommand){
	pidStack[pidStackSize] = cpid;
	strcpy(processName[pidStackSize], syscommand);
	pidStackSize++;
	fprintf(stderr,"[%d] %d\n",pidStackSize, cpid);
}