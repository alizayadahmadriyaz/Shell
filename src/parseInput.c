#include <string.h>



void parseInput(char *tok, char arg[100][100], int* bg, int *iredir, char *ifile, int *oredir, char *ofile, int *append, int *ispipe){
	int i =0 ;
	while(tok != NULL){
		if(strcmp(tok,"&") == 0){
			*bg = 1;
		}
		else if(strcmp(tok,"<") == 0){
			*iredir = 1;
			tok = strtok(NULL," ");
			strcpy(ifile,tok);
		}
		else if(strcmp(tok,">") == 0){
			*oredir = 1;
			tok = strtok(NULL," ");
			strcpy(ofile,tok);
		}
		else if(strcmp(tok,">>") == 0){
			*oredir = 1;
			*append = 1;
			tok = strtok(NULL," ");
			strcpy(ofile,tok);
		}
		else if(strcmp(tok,"|") == 0){
			*ispipe = 1;
		}

		else{
			strcpy(arg[i],tok);
			i++;
		}
		tok = strtok(NULL," ");
	}
	*arg[i] = '\0';
}

