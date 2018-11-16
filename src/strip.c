#include <string.h>
#include <stdlib.h>
#include "mylimits.h"
#include <ctype.h>

void strstrip(char *s){
	size_t size;
	char *end;
	size = strlen(s);

	if(!size){
		return;
	}

	end = s + size - 1;
	while(end >= s && isspace(*end))
			end--;
	*(end + 1) = '\0';

	while(isspace(*s)){
		for(int i=1;s[i]!= 0;i++){
			s[i-1] = s[i];
		}
		end --;
	}
	*(end + 1) = '\0';
}