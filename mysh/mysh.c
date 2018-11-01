#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFSIZE 512

int argsplit(char *str, char *argv[]){
	int i, argc = 0;

	while(*str != '\0'){
		while(*str == ' ' || *str == '\t')*str++ = '\0';
		argv[argc] = str;
		argc++;
		while(*str != ' ' && *str != '\t' && *str != '\0')str++;
	}
	argv[argc] = NULL;
	
	//for(i=0; i<=argc; i++)printf(" cmd[%d]: %s\n",i,argv[i]);

	return argc;
}

void chop(char *str){ str[strlen(str)-1]='\0';}

int main(){
	pid_t ret;
	int status, cnt = 0;
	char str[BUFSIZE];
	char *argv[20];

	while(1){
		printf("mysh[%d]> ",cnt++);
		if(fgets(str,BUFSIZE,stdin) == NULL){
			perror(str);
			exit(EXIT_FAILURE);
		}
		chop(str);
		argsplit(str,argv);
		if(strcmp(argv[0],"quit") == 0)break;
		ret = fork();
		if(ret == 0){
			execvp(argv[0],argv);
			printf(" mysh: Unknown Command\n");
			exit(EXIT_FAILURE);
		}
		else if(ret > 0)wait(&status);
		else{
			perror("fork");
			exit(EXIT_FAILURE);
		}
	}
}
