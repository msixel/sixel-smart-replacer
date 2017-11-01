/*
 ============================================================================
 Name        : sequence.c
 Author      : Marcos Sixel
 Version     :
 Copyright   : RSI Informatica
 Description : Manipulacao de sequencias para geracao de arquivos
 ============================================================================
 */

#include "sequence.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const long SEQUENCE_INITIAL_VALUE = 0;

/*
 * rotina para verificar se arquivo existe e inicializa-lo se necessario
 */
bool initializeSequenceFile(char* sequenceFilename) {
	if ((F_OK == access(sequenceFilename, F_OK))) {
		return true;
	} else {
		FILE* sequenceFile;
		if ((sequenceFile = fopen(sequenceFilename, "w")) == NULL) {
			fprintf(stderr, "Arquivo %s não pode ser aberto para escrita.\n", sequenceFilename);
			return false;
		}
		fprintf(sequenceFile, "%ld\n", SEQUENCE_INITIAL_VALUE);
		fclose(sequenceFile);
		return true;
	}
}

/*
 * rotina para obter valor atual da sequence, incrementar,
 * persistir e retornar o valor incrementado
 */
long sequenceNewValue(char* sequenceFilename) {
	FILE* sequenceFile;
	long sequenceValue;

	if ((sequenceFile = fopen(sequenceFilename, "r+")) == NULL) {
		fprintf(stderr, "Arquivo %s não pode ser aberto para leitura e escrita.\n", sequenceFilename);
		return -1;
	}

	fscanf(sequenceFile, "%ld\n", &sequenceValue);
	sequenceValue++;
	//retornando ao inicio do arquivo
	rewind(sequenceFile);//	fseek(sequenceFile, 0, SEEK_SET);
	fprintf(sequenceFile, "%ld\n", sequenceValue);
	fclose(sequenceFile);

	return sequenceValue;
}




