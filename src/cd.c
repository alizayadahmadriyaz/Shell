#include "cd.h"
#include <unistd.h>
#include <stdio.h>

void cd(char *newdir){
	if(chdir(newdir)){
		perror("cd");
	}
}


	
