#include<stdio.h>
#include<stdlib.h>
#include "WAV.h"

int main(int ac, char *av[]){
	short **data;					//variable to save wave data
	char output[261], source[256];
	size_t size, total_size=0;
	int i, av_length;
	Fmt format;						//format of output file

	if(ac != 2){
		printf("usage : %s number\n",av[0]);
		printf("wav file reading the first argument will be created.\n");
		printf("Ex. [%s 123] will create [read_123.wav].\n",av[0]);
		exit(EXIT_FAILURE);
	}

	//Allocating memory for wave data (desu_02.wav included)
	av_length = strlen(av[1]);
	if((data = malloc(sizeof(short *) * av_length+1)) == NULL){
		perror("Failed to allocate memory");
		exit(EXIT_FAILURE);
	}

	//Creating wave data
	for(i=0; i<=av_length; i++){
		if(i == av_length)sprintf(source,"sound/desu_02.wav");
		else sprintf(source,"sound/num00%c_01.wav",av[1][i]);
		printf("Opening : %s\n",source);
		size = wavRead(source, &data[i], &format, false, false);
		total_size += size;
	}

	//Writing wav file
	sprintf(output,"read_%s.wav",av[1]);
	printf("Writing Read Aloud file : %s\n",output);
	wavWrite(output,*data,format,total_size);

	return EXIT_SUCCESS;
}
