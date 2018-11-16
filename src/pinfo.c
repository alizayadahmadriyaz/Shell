#include <unistd.h>
#include "pinfo.h"
#include <limits.h>
#include "mylimits.h"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>


void getStatus(int pid, char *status){
	char str[MAXN];	
	sprintf(str,"%d",pid);
	char procStatusFile[PATH_MAX];
	
	strcpy(procStatusFile,"/proc/");
	strcat(procStatusFile, str);
	strcat(procStatusFile, "/status");

	int fd = open(procStatusFile, O_RDONLY);
	if(fd == -1){
		perror("pinfo:");
		return;
	}
	char buffer[MAXN];
	read(fd,buffer,MAXN);
	char *pt = strstr(buffer, "State");
	pt += 6;
	while(*pt != '\n'){
		char c = *pt;
		strcat(status,&c);
		pt++;
	}
	close(fd);
}

void pinfo(char *pidc){
	int pid;
	if(!pidc){
		pid = getpid();
	}
	else{
		pid = atoi(pidc);
	}

	printf("pid: %d\n", pid);
	char str[MAXN];	
	sprintf(str,"%d",pid);
	char procStatusFile[PATH_MAX];
	char procMapFile[PATH_MAX];
	strcpy(procStatusFile,"/proc/");
	strcat(procStatusFile, str);

	strcpy(procMapFile,procStatusFile);
	strcat(procStatusFile, "/status");
	strcat(procMapFile,"/maps");

	// printf("%s\n",procStatusFile);
	int fd = open(procStatusFile, O_RDONLY);
	if(fd == -1){
		perror("pinfo:");
		return;
	}
	char buffer[MAXN];
	read(fd,buffer,MAXN);
	char *pt = strstr(buffer, "State");
	while(*pt != '\n'){
		printf("%c",*pt);
		pt++;
	}
	printf("\n");

	pt = strstr(buffer, "VmSize");
	while(*pt != '\n'){
		printf("%c",*pt);
		pt++;
	}
	printf("\n");
	close(fd);

	fd = open(procMapFile,O_RDONLY);
	if(fd == -1){
		perror("pinfo:");
		return;
	}
	read(fd,buffer,MAXN);
	pt = strstr(buffer,"/");
	printf("Executable Path: ");
	while(*pt != '\n'){
		printf("%c",*pt);
		pt++;
	}
	printf("\n");
}