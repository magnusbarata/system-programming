#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFSIZE 512

int argsplit(char *str, char *argv[], int *ppos, int *pnum){
	int i, argc = 0;

	while(*str != '\0'){
		while(*str == ' ' || *str == '\t')*str++ = '\0';
		if(*str == '|'){
			argv[argc] = NULL;
			(*pnum)++;
			ppos[(*pnum)] = argc+1;	//Locating position of pipe between args 
		}
		else argv[argc] = str;
		argc++;
		while(*str != ' ' && *str != '\t' && *str != '\0')str++;
	}
	argv[argc] = NULL;
	
//	for(i=0; i<=argc; i++)printf(" cmd[%d]: %s\n",i,argv[i]);

	return argc;
}

void chop(char *str){ str[strlen(str)-1]='\0';}

int main(){
	pid_t ret;
	int status, fdprev[2], fdnext[2], cnt = 0;
	int i, argc, ppos[9], pnum, end;
	char str[BUFSIZE];
	char *argv[20];

	while(1){
		printf("mysh[%d]> ",cnt++);
		if(fgets(str,BUFSIZE,stdin) == NULL){
			perror(str);
			exit(EXIT_FAILURE);
		}
		chop(str);
		ppos[0] = 0;	pnum = 0;	//Resetting number of pipe and position
		argc = argsplit(str,argv,ppos,&pnum);
		if(strcmp(argv[0],"quit") == 0)break;

		//Executing cmd 
		for(i = 0; i <= pnum; i++){ 
			//Creating next and previous pipe when needed
			if(pnum > 0 && i < pnum){ 
				if(pipe(fdnext) < 0){
					perror("pipe");
					exit(EXIT_FAILURE);
				}
			}

			//Exec on child process
			ret = fork();
			if(ret == 0){
				if(i < pnum){
					//Redirecting stdout to next command
					close(1);
					if(dup(fdnext[1]) != 1){
						perror("dup next pipe failed");
						close(fdnext[1]);
						exit(EXIT_FAILURE);
					}
					//Closing pipe
					close(fdnext[0]);
					close(fdnext[1]);
				}
				if(i > 0){
					//Redirecting stdin of next command
					close(0);
					if(dup(fdprev[0]) != 0){
						perror("dup previous pipe failed");
						close(fdprev[0]);
						exit(EXIT_FAILURE);
					}
					//Closing pipe
					close(fdprev[0]);
					close(fdprev[1]);
				}
				execvp(argv[ppos[i]],&argv[ppos[i]]);
				printf(" mysh: Unknown Command\n");
				exit(EXIT_FAILURE);
			}
			//else if(ret > 0)wait(&status); 	<- NO NEED TO WAIT! (PIPE CLOG)
			else if(ret < 0){
				perror("fork");
				exit(EXIT_FAILURE);
			}

			if(i > 0){
				close(fdprev[0]);
				close(fdprev[1]);
			}

			//Reassigining previous pipe
			fdprev[0] = fdnext[0];
			fdprev[1] = fdnext[1];
		}

		//Waiting for child process
		while((end = waitpid(0,&status,0)) > 0);
	}
}
