/*
 ============================================================================
 Name        : fileprocessing.c
 Author      : Marcos Sixel
 Version     :
 Copyright   : RSI Informatica
 Description : Processamento dos arquivos para substituicao
 ============================================================================
 */


#include "fileprocessing.h"
#include "sequence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool replaceSequenceValue (char* rowValue, char* sequenceFilename, int startPosition, int endPosition) {
	long sequenceValue;
	int blockLength;
	char* blockBuffer;

	if (endPosition > strlen(rowValue)) {
		fprintf(stderr, "BLOCO NAO LOCALIZADO NA LINHA [POSICAO_INICIAL: %d, POSICAO_FINAL: %d]", startPosition, endPosition);
		return false;
	}

	if ((sequenceValue = sequenceNewValue(sequenceFilename)) == -1)
		return false;

	//criando novo bloco
	blockLength = (endPosition - startPosition + 1) * sizeof(char);
	blockBuffer = (char*) malloc(blockLength);
	sprintf(blockBuffer, "%0*ld", blockLength, sequenceValue);
	memcpy(rowValue + ((startPosition - 1) * sizeof(char)), blockBuffer, blockLength);
	free(blockBuffer);
	return true;
}


bool processFile(char* sourceFilename,
		char* sequenceFilename,
		int rowNumber,
		int startPosition,
		int endPosition,
		char* targetFilename) {

	FILE* sourceFile;
	FILE* targetFile;
	char rowValue[1024];
	int contador;

	//abrindo arquivos
	if ((sourceFile = fopen(sourceFilename, "r")) == NULL) {
		fprintf(stderr, "Arquivo %s não pode ser aberto para leitura.\n", sourceFilename);
		return false;
	}
	if ((targetFile = fopen(targetFilename, "w")) == NULL) {
		fprintf(stderr, "Arquivo %s não pode ser aberto para escrita.\n", targetFilename);
		return false;
	}

	//iterando arquivos e fazendo substituicoes
	rowNumber--;
	for (contador=0; fscanf(sourceFile, "%s\n", rowValue)!= EOF; contador++) {
		if (rowNumber == contador ) {
			//localizada linha para receber tratamento
			if (!replaceSequenceValue(rowValue, sequenceFilename, startPosition, endPosition)) {
				fclose(sourceFile);
				fclose(targetFile);
				return false;
			}
		}
		//escrevendo arquivo de saida
		fprintf(targetFile, "%s\n", rowValue);
	}

	//fechando arquivos
	fclose(sourceFile);
	fclose(targetFile);

	return true;
}
