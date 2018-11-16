#include <unistd.h>
#include "clock.h"
#include <limits.h>
#include "mylimits.h"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

void clk(int interval, int times){
	int fd1 = open("/sys/class/rtc/rtc0/date",O_RDONLY);
	int fd2 = open("/sys/class/rtc/rtc0/time",O_RDONLY);

	int flag = times == 0;

	while(times-- || flag){
		lseek(fd1,0,SEEK_SET);
		lseek(fd2,0,SEEK_SET);
		char buffer[50];
		char buffer2[50];
		int i = read(fd1,buffer,50);
		buffer[i-1] = 0;
		i = read(fd2,buffer2,50);
		buffer2[i] = 0;

		printf("%s %s",buffer, buffer2);
		int sta = sleep(interval);
		if(sta){
			break;
		}
	}

}