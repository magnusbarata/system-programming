all: showWavDetail simpleTTS genDTMF
showWavDetail: WAV.h showWavDetail.c
	gcc -o showWavDetail showWavDetail.c
simpleTTS: WAV.h simpleTTS.c
	gcc -o simpleTTS simpleTTS.c
genDTMF: WAV.h genDTMF.c
	gcc -o genDTMF genDTMF.c -lm
