#include<stdio.h>
#include<stdlib.h>
#include "WAV.h"

int main(int ac, char *av[]){
	short *dummy = NULL;	//dummy data pointer
	Fmt dummy_fmt;			//dummy format variable

	if(ac != 2){
		printf("usage : %s filename.wav\n",av[0]);
		printf("Shows the detail of [filename.wav].\n");
		exit(EXIT_FAILURE);
	}

	wavRead(av[1], &dummy, &dummy_fmt, false, true);

	return EXIT_SUCCESS;
}
