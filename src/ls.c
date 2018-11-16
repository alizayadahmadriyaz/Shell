#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "ls.h"
#include "mylimits.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

void ls(char *dir, char *options){
	DIR *directory = opendir(dir);
	if(!directory){
		perror("ls");
		return;
	}
	struct dirent *cur_file;

	int aflag = 0;
	int lflag = 0;

	if(strstr(options,"a")){
		aflag = 1;
	}
	if(strstr(options,"l")){
		lflag = 1;
	}


	while((cur_file = readdir(directory)) != NULL){
		if(!aflag && cur_file->d_name[0] == '.'){
			continue;
		}
		if(!lflag){
			printf("%s\n", cur_file->d_name);
			
		}
		else{
			char buffer[MAXN];
			struct stat fileStat; 
			stat(cur_file->d_name, &fileStat);
			printf("%s",(S_ISDIR(fileStat.st_mode)) ? "d" : "-");
			printf("%s",(fileStat.st_mode & S_IRUSR) ? "r" : "-");
			printf("%s",(fileStat.st_mode & S_IWUSR) ? "w" : "-");
			printf("%s",(fileStat.st_mode & S_IXUSR) ? "x" : "-");
			printf("%s",(fileStat.st_mode & S_IRGRP) ? "r" : "-");
			printf("%s",(fileStat.st_mode & S_IWGRP) ? "w" : "-");
			printf("%s",(fileStat.st_mode & S_IXGRP) ? "x" : "-");
			printf("%s",(fileStat.st_mode & S_IROTH) ? "r" : "-");
			printf("%s",(fileStat.st_mode & S_IWOTH) ? "w" : "-");
			printf("%s",(fileStat.st_mode & S_IXOTH) ? "x" : "-");

			printf(" ");
			printf("%ld ", fileStat.st_nlink);

			struct passwd *pwd;
			struct group *grp;

			if ((pwd = getpwuid(fileStat.st_uid)) != NULL){
				printf("%s ", pwd->pw_name);   
			}
			printf(" ");
			  
			if ((grp = getgrgid(fileStat.st_gid)) != NULL){
				printf("%s ", grp->gr_name); 
			}

			printf(" ");
			printf("%ld ", fileStat.st_size);

			printFileModTime(fileStat);

			printf(" ");
			printf("%s ", cur_file->d_name);
			printf("\n");
		}
	}
	
	closedir(directory);
}



	
void printFileModTime(struct stat metadata){
  int success;
  char timeBuff[14];
  time_t current = time (NULL);
  
  struct tm *temp;

  // Extract tm struct from metadata
  temp = localtime(&metadata.st_mtime);
  if (temp == NULL) perror("localtime");  
  struct tm fileBuff = *temp;

  // Extract tm struct from current time
  temp = localtime(&current);
  if (temp == NULL) perror("localtime");  
  struct tm currentBuff = *temp;

  
  if (currentBuff.tm_year == fileBuff.tm_year) // modified in current year
    {      
      success = strftime(timeBuff, sizeof timeBuff, "%h %d %H:%M", &fileBuff);
      if(!success) perror("strftime");
      printf("%s ", timeBuff);
    }
  else //modified in a previous year
    {
      success = strftime(timeBuff, sizeof timeBuff, "%h %d %Y", &fileBuff);
      if(!success) perror("strftime");
      printf("%s ", timeBuff);
    }
}