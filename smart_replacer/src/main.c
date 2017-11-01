/*
 ============================================================================
 Name        : main.c
 Author      : Marcos Sixel
 Version     :
 Copyright   : RSI Informatica
 Description : Ponto de entrada do programa
 ============================================================================
 */


#include "fileprocessing.h"
#include "sequence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char* SEQUENCE_FILE_EXTENSION = ".sequence";


int main(int argc, char *argv[]) {
	char* sequenceFilename;
	char* sourceFilename;
	char* targetFilename;
	long rowNumber;
	long startPosition;
	long endPosition;

	if (argc!=7) {
		fprintf (stderr, "Usage: rsi_reuse_file_test_data ARQUIVO_ORIGEM ARQUIVO_DESTINO SEQUENCE_NAME LINHA POSICAO_INICIAL POSICAO_FINAL");
		return EXIT_FAILURE;
	}

	//recebendo parametros
	sourceFilename = argv[1];
	targetFilename = argv[2];
	sequenceFilename = (char*) calloc(strlen (argv[3]) + strlen(SEQUENCE_FILE_EXTENSION), sizeof(char));// alocando memoria dinamicamente do heap
	sequenceFilename = strcpy(sequenceFilename, argv[3]);
	sequenceFilename = strcat(sequenceFilename, SEQUENCE_FILE_EXTENSION);
	rowNumber = strtol(argv[4], NULL, 0);
	startPosition = strtol(argv[5], NULL, 0);
	endPosition = strtol(argv[6], NULL, 0);

	//validando parametros
	if (startPosition < 1) {
		fprintf (stderr, "POSICAO_INICIAL INVALIDA");
		return EXIT_FAILURE;
	}
	if (startPosition > endPosition) {
		fprintf (stderr, "POSICAO_FINAL DEVE SER MAIOR QUE POSICAO_INICIAL");
		return EXIT_FAILURE;
	}

	//fprintf(stdout, "sequenceName: %s\nrowNumber: %ld\nstartPosition: %ld\nendPosition: %ld\n", sequenceFilename, rowNumber, startPosition, endPosition);

	if (!initializeSequenceFile(sequenceFilename))
		return EXIT_FAILURE;

	if (!processFile(sourceFilename, sequenceFilename, rowNumber, startPosition, endPosition, targetFilename))
		return EXIT_FAILURE;

	free(sequenceFilename);//desalocando memoria do heap
	return EXIT_SUCCESS;
}


