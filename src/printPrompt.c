#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include "mylimits.h"
#include <string.h>

void printPrompt(char *curpath, char *home){
	char hostname[HOST_NAME_MAX];
	char username[LOGIN_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);
	getlogin_r(username, LOGIN_NAME_MAX);

	if(strlen(curpath) < strlen(home)){
		printf("<%s@%s:%s> ",username,hostname, curpath);
	}
	else{
		printf("<%s@%s:~%s> ",username,hostname, curpath+strlen(home));
		
	}
	


}