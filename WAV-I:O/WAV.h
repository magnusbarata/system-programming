#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifndef WAV_H_
#define WAV_H_

typedef enum{false, true} bool;

//wav file identifier
typedef struct riff{
	char riff[4];
	unsigned int data_size;
	char wave[4];
} Riff;

//Head of each chunk
typedef struct chunkhead{
	char name[4];
	unsigned int chunk_size;
} ChunkHead;

//Format of wav
typedef struct fmt{
	unsigned short type;
	unsigned short channel;
	unsigned int s_rate;
	unsigned int speed;
	unsigned short b_size;
	unsigned short bit_qty;
} Fmt;

//Function to write wav file. Return the size of data successfully written.
size_t wavWrite( const char *filename, const short *data,
				 const Fmt format, const size_t data_size){
	FILE *fp;
	Riff r;
	ChunkHead chnkHead;
	size_t written;

	if((fp = fopen(filename,"wb")) == NULL){
		perror(filename);
		exit(EXIT_FAILURE);
	}

	//Writing wav file identifier
	strncpy(r.riff,"RIFF",4); strncpy(r.wave,"WAVE",4);
	r.data_size = 4 + data_size + 2*sizeof(ChunkHead) + sizeof(Fmt);
	fwrite(&r,sizeof(Riff),1,fp);

	//Writing wav format
	strncpy(chnkHead.name,"fmt ",4); chnkHead.chunk_size = 16;
	fwrite(&chnkHead,sizeof(ChunkHead),1,fp);
	fwrite(&format,sizeof(Fmt),1,fp);

	//Writing data
	strncpy(chnkHead.name,"data",4); chnkHead.chunk_size = data_size;
	fwrite(&chnkHead,sizeof(ChunkHead),1,fp);
	written = fwrite(data,data_size,1,fp);

	fclose(fp);
	return written;
}

//Function to read wav file. Return the size of data successfully read.
size_t wavRead( const char *filename, short **waveData, Fmt *format,
				const bool showFormat, const bool showAll){
	FILE *fp;
	unsigned short extraSize;	//extra size of fmt chunk
	unsigned int waveSize;		//total size of data chunk
	char dummy[5];
	Riff riffHead;
	ChunkHead chnkHead;

	if((fp = fopen(filename,"rb")) == NULL){
		perror(filename);
		exit(EXIT_FAILURE);
	}

	//Reading wav file identifier
	fread(&riffHead,sizeof(Riff),1,fp);
	if(showFormat || showAll){
		printf("Wav Filename : %s\n",filename);
		printf("------------------HEADER---------------------\n");
		printf("Wav Data Size\t\t: %u [byte]\n",riffHead.data_size);
	}

	//Reading chunks
	while(fread(&chnkHead,sizeof(ChunkHead),1,fp) > 0){
		//Putting null character on chunk name before using strcmp
		strncpy(dummy,chnkHead.name,4); dummy[4]='\0';

		//Reading wav format
		if(strcmp("fmt ",dummy)==0){
			fread(format,sizeof(Fmt),1,fp);
			if(showFormat || showAll){
				printf("---------------------------------------------\n");
				printf("Chunk Name\t\t: [%s]\n",dummy);
				printf("Chunk Size\t\t: %u [byte]\n",chnkHead.chunk_size);
				printf("Format\t\t\t: %hu\n",format->type);
				printf("Channel\t\t\t: %hu [1: monaural, 2: stereo]\n",
						format->channel);
				printf("Sampling Rate\t\t: %u [Hz]\n",format->s_rate);
				printf("Speed\t\t\t: %u [byte/sec]\n",format->speed);
				printf("Block Size\t\t: %hu [byte/sample]\n",format->b_size);
				printf("Bit Quantization\t: %hu [bit/sample]\n",
						format->bit_qty);
			}

			//Checking for format chunk expansion
			if(chnkHead.chunk_size > 16){
				fread(&extraSize,sizeof(extraSize),1,fp);
				fseek(fp,extraSize,SEEK_CUR);
			}
		}

		//Reading data chunk
		else if(strcmp("data",dummy)==0){
			waveSize = chnkHead.chunk_size;
			if(showFormat || showAll){
				printf("---------------END OF HEADER-----------------\n");
				printf("-------------------DATA----------------------\n");
				printf("Wave Data Size\t\t: %u [byte]\n",waveSize);
				printf("----------------END OF DATA------------------\n");
			}
			if((*waveData = malloc(waveSize)) == NULL){
				perror("Failed to allocate memory");
				exit(EXIT_FAILURE);
			}
			fread(*waveData, waveSize,1,fp);
		}

		//Reading other chunks
		else{
			if(showAll){
				printf("---------------------------------------------\n");
				printf("Chunk Name\t\t: [%s]\n",dummy);
				printf("Chunk Size\t\t: %u [byte]\n",chnkHead.chunk_size);
			}
			fseek(fp,chnkHead.chunk_size,SEEK_CUR);
		}
	}

	fclose(fp);
	return waveSize;
}

#endif
