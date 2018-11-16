#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "execute.h"
#include "ls.h"
#include "cd.h"
#include "mylimits.h"
#include "pinfo.h"
#include "clock.h"
#include "checkProc.h"
#include "strip.h"
#include "printPrompt.h"
#include "parseInput.h"



int mypid;
int mypgrp;
char home[PATH_MAX], curpath[PATH_MAX], prevpath[PATH_MAX];
char syscommand[50];
void programLoop();



void signal_handler(int sig){
	int callerpid = getpid();
	if(callerpid == mypid){
		if(sig == SIGTSTP){
			int childid = waitpid(- mypgrp, NULL, WNOHANG | WUNTRACED);
			printf("childpid %d\n",childid);
			if(childid){
				addProc(childid,syscommand);
			}
		}
	}
	// signal(sig, signal_handler);
}

int stdincopy,stdoutcopy;

void iredirect(char *ifile){
	int ifd = open(ifile,O_RDONLY);
	if(ifd<0){
		perror("input redirect :");
		return;
	}
	dup2(ifd,STDIN_FILENO);
}

void oredirect(char *ofile, int append){
	int ifd;
	if(append){
		ifd = open(ofile,O_WRONLY | O_CREAT | O_APPEND, 0644);
	}
	else{
		ifd = open(ofile,O_WRONLY | O_CREAT | O_TRUNC, 0644);
	}
	if(ifd<0){
		perror("output redirect :");
		return;
	}
	dup2(ifd,STDOUT_FILENO);	
}


void clearRedirect(){
	close(STDOUT_FILENO);
	close(STDIN_FILENO);
	dup2(stdincopy, STDIN_FILENO);
	dup2(stdoutcopy, STDOUT_FILENO);
}


void piperead(){
	int ifd = open("tmpinfile.txt", O_RDONLY);
	dup2(ifd,STDIN_FILENO);
}

void pipewrite(){
	int ofd = open("tmpoutfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	dup2(ofd,STDOUT_FILENO);
}

void swappipe(){
	// rename("tmpinfile.txt", "tmpoutfile2.txt");
	rename("tmpoutfile.txt", "tmpinfile.txt");
	// rename("tmpoutfile2.txt", "tmpoutfile.txt");
}


void programLoop(){
	while(1){
		checkProc();	
		printPrompt(curpath,home);

		char command[MAXN] = {0};
		scanf("%[^\n]",command);
		scanf("%*c");
		strstrip(command);
		if(strlen(command) == 0){
			continue;
		}

		char *saveptr1,*saveptr2;
		char *mainTok = strtok_r(command,";",&saveptr1);

		char cmdlist[50][MAXN];
		int cmdListTop = 0;
		while(mainTok != NULL){
			// printf("%s\n", mainTok);
			char *twotok = strtok_r(mainTok,"|",&saveptr2);
			while(twotok != NULL){
				strcpy(cmdlist[cmdListTop++],twotok);
				twotok = strtok_r(NULL,"|", &saveptr2);
				if(twotok != NULL){
					strcat(cmdlist[cmdListTop-1]," |");
				}
			}
			// strcpy(cmdlist[cmdListTop++],mainTok);
			mainTok = strtok_r(NULL,";",&saveptr1);
		}
		
		int ispipe = 0, prevpipe =0;
		for(int j=0;j<cmdListTop;j++){
			strstrip(cmdlist[j]);
			if(strlen(cmdlist[j]) == 0){
				continue;
			}

			char *tok = strtok(cmdlist[j]," ");
			char comm[100][100];
			char ifile[PATH_MAX];
			char ofile[PATH_MAX];
			int iredir=0,oredir=0,bg =0, append = 0;
			ispipe= 0;
			parseInput(tok,comm,&bg,&iredir,ifile,&oredir,ofile, &append, &ispipe);
			// printf("Command : %s\n prevpipe = %d, ispipe = %d\n",cmdlist[j], prevpipe, ispipe);

			if(prevpipe){
				piperead();
			}
			if(ispipe){
				pipewrite();
			}
			swappipe();
			prevpipe = ispipe;

			tok = comm[0];
			int curcomm = 0;

			if(iredir){
				iredirect(ifile);
			}
			if(oredir){
				oredirect(ofile,append);
			}

			if(strcmp(tok,"ls") == 0){
				char tmppath[PATH_MAX] = ".";
				char options[10] = "";
				tok = comm[++curcomm];
				while(*tok != 0){
					if(tok[0] == '-'){
						strcat(options,&tok[1]);
					}
					else{
						if(*tok == '~'){
							strcpy(tmppath,home);
							strcat(tmppath,"/");
							tok++;
							strcat(tmppath,tok);
						}
						else{
							strcpy(tmppath,tok);
						}
					}
					curcomm+=1;
					tok = comm[curcomm];
				}
				ls(tmppath,options);
			}
			else if(strcmp(tok,"setenv")==0){
				tok = comm[++curcomm];
				if(*tok == 0){
					printf("USAGE setenv [var] [value]\n");
				}
				else{
					char tmp[MAXN];
					strcpy(tmp,tok);
					tok = comm[++curcomm];
					if(*tok != 0){
						setenv(tmp,tok,1);
					}
					else{
						setenv(tmp,"",1);
					}
				}
			}
			else if(strcmp(tok,"unsetenv")==0){
				tok = comm[++curcomm];
				if(*tok == 0){
					printf("USAGE unsetenv [var]\n");
				}
				else{
					unsetenv(tok);
				}	
			}
			else if(strcmp(tok,"jobs")==0){
				jobs();
			}
			else if(strcmp(tok,"kjob")==0){
				tok = comm[++curcomm];
				if(*tok == 0){
					printf("USAGE kjob [JOB] [SIGNUM]\n");
				}
				else{
					int ind = atoi(tok);
					tok = comm[++curcomm];
					int signum ;
					if(*tok != 0){
						signum = atoi(tok);
					}
					else{
						signum = SIGINT;
					}
					kjob(ind-1, signum);
				}
			}
			else if(strcmp(tok,"overkill")==0){
				overkill();
			}
			else if(strcmp(tok,"fg")==0){
				tok = comm[++curcomm];
				int ind = atoi(tok);
				fg(ind-1, mypgrp);
			}
			else if(strcmp(tok,"bg")==0){
				tok = comm[++curcomm];
				int ind = atoi(tok);
				bgco(ind-1);
			}
			else if(strcmp(tok,"cd")==0){
				tok = comm[++curcomm];
				if(*tok == 0){
					cd(home);
				}
				else{
					char newpath[PATH_MAX]="";
					if(*tok == '~'){
						strcat(newpath,home);
						strcat(newpath,"/");
						tok++;
					}
					strcat(newpath,tok);
					cd(newpath);
				}
				strcpy(prevpath, curpath);
				getcwd(curpath,PATH_MAX);
			}
			else if(strcmp(tok,"pwd")==0){
				printf("%s\n", curpath);
			}
			else if(strcmp(tok,"echo")==0){
				tok = comm[++curcomm];
				while(*tok != 0){
					if(tok[0] == '$'){
						tok++;
						if(getenv(tok) != NULL){
							printf("%s ",getenv(tok));
						}
					}
					else{
						printf("%s ",tok);
					}
					tok = comm[++curcomm];
				}
				printf("\n");
			}
			else if(strcmp(tok,"pinfo") == 0){
				tok = comm[++curcomm];
				if(*tok != 0){
					pinfo(tok);
				}
				else{
					pinfo(NULL);
				}
			}
			else if(strcmp(tok,"remindme") == 0){
				char arg[10][100];
				char *args[10];
				int i =0;
				int bg = 1;

				strcpy(arg[0],"sleep");
				tok = comm[++curcomm];
				strcpy(arg[1], tok);
				args[0] = arg[0];
				args[1] = arg[1];
				args[2] = (char *)0;
				
				char reminder[50];
				strcpy(reminder, "Reminder: ");
				tok = comm[++curcomm];
				while(*tok != 0){
					strcat(reminder,tok);
					strcat(reminder," ");
					tok = comm[++curcomm];
				}
				
				int cpid = execute(args[0], args, bg);

				if(cpid != -1){
					addProc(cpid, reminder);
				}
			}
			else if(strcmp(tok, "clock")==0){
				tok = comm[++curcomm];
				int times = 0;
				int interval = 1;
				while(*tok != 0){
					if(strcmp(tok,"-n")==0){
						tok = comm[++curcomm];
						times = atoi(tok);
					}
					else if(strcmp(tok,"-t")==0){
						tok = comm[++curcomm];
						interval = atoi(tok);
					}
					tok = comm[++curcomm];
				}
				clk(interval, times);
			}
			else if(strcmp(tok,"quit") == 0){
				return;
			}
			else{
				
				char *args[100];
				int i=0;
				while(*tok != 0){
					args[i++] = tok;
					tok = comm[++curcomm];
				}
				
				args[i] = (char *)0;
				int cpid = execute(args[0], args, bg);

				
				syscommand[0] =0;
				for(int j=0;j<i;j++){
					strcat(syscommand,args[j]);
					strcat(syscommand," ");
				}

				if(cpid != -1){
					addProc(cpid,syscommand);
				}
			}	
			clearRedirect();
		}
	}
}

int main(){
	mypid = getpid();
	mypgrp = getpgrp();
	getcwd(home,PATH_MAX);
	strcpy(curpath,home);
	strcpy(prevpath,home);

	// signal(SIGINT, signal_handler);
	signal(SIGINT, SIG_IGN);
	signal (SIGQUIT, SIG_IGN);
	// signal (SIGTSTP, signal_handler);
	signal (SIGTSTP, SIG_IGN);
	signal (SIGTTIN, SIG_IGN);
	signal (SIGTTOU, SIG_IGN);
	signal (SIGCHLD, SIG_IGN);

	stdincopy = dup(STDIN_FILENO);
	stdoutcopy = dup(STDOUT_FILENO);
	

	// printf("FG pgid: %d",tcgetpgrp(1));
	// printf("My Shell pgid: %d",getpgrp());


	programLoop();

	close(stdincopy);
	close(stdoutcopy);
}