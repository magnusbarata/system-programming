#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "WAV.h"

int main(int ac, char *av[]){
	const int AMP = 10000;			//amplitude (Volume) of output
	const double DURATION = 0.2;	//duration of each tone
	double in1, in2;
	short *data;					//variable to save wave data
	char output[261];
	size_t data_size;				//wave data size
	Fmt format;						//format of output file
	int i, j, k, l, av_length, tone_size;
	//dual-tone multi frequency variable
	const int F1[4] = {697,770,852,941},
			  F2[3] = {1209,1336,1477};

	if(ac != 2){
		printf("usage : %s number\n",av[0]);
		printf("wav file simulating DTMF will be created.\n");
		printf("Ex. [%s 123] will create [dial_123.wav].\n",av[0]);
		exit(EXIT_FAILURE);
	}

	//Defining output file format
	format.type = 1;
	format.channel = 1;
	format.s_rate = 44100;
	format.bit_qty = 16;
	format.b_size = format.channel * format.bit_qty / 8;
	format.speed = format.channel * format.s_rate * format.bit_qty / 8;
	
	//Calculating each tone size (0.1 seconds between tones included)
	tone_size = format.s_rate * (DURATION + 0.1);

	//Allocating memory for wave data
	av_length = strlen(av[1]);
	data_size = sizeof(short) * av_length * tone_size;
	if((data = malloc(data_size)) == NULL){
		perror("Failed to allocate memory");
		exit(EXIT_FAILURE);
	}
	
	//Creating wave data
	for(i=0; i<av_length; i++){
		//Switching between frequencies
		switch(av[1][i]){
			case '0' : 
				j = 3; k = 1; break;
			case '1' : 
				j = k = 0; break;
			case '2' :
				j = 0; k = 1; break;
			case '3' :
				j = 0; k = 2; break;
			case '4' :
				j = 1; k = 0; break;
			case '5' : 
				j = k = 1; break;
			case '6' :
				j = 1; k = 2; break;
			case '7' :
				j = 2; k = 0; break;
			case '8' : 
				j = 2; k = 1; break;
			case '9' :
				j = k = 2; break;
			case '*' :
				j = 3; k = 0; break;
			case '#' : 
				j = 3; k = 2; break;
			default :
				perror("Unknown symbol");
				exit(EXIT_FAILURE);
		}

		printf("Generating Tone '%c' : f1=%d, f2=%d\n",av[1][i],F1[j],F2[k]);
		for(l=0; l<tone_size; l++){
			if(l >= DURATION*format.s_rate){
				in1 = in2 = 0;
			}else{
				in1 = sin(l*F1[j]*2*M_PI/format.s_rate);
				in2 = sin(l*F2[k]*2*M_PI/format.s_rate);
			}
			*(data+i*tone_size+l) = AMP * (in1 + in2);
		}
	}
	
	//Writing wav file
	sprintf(output,"dial_%s.wav",av[1]);
	printf("Writing Dial Sound File : %s\n",output);
	wavWrite(output,data,format,data_size);

	return EXIT_SUCCESS;
}
