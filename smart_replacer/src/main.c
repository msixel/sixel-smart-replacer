/*
 ============================================================================
 Name        : main.c
 Author      : Marcos Sixel
 Version     :
 Copyright   : RSI Informatica
 Description : Ponto de entrada do programa
 ============================================================================
 */

#include "arguments.h"
#include "arguments_types.h"
#include "fileprocessing.h"
#include "sequence.h"
#include "arguments.h"
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
	struct arguments arguments;

	if (!parseParameters(argc, argv, &arguments)) {
		return EXIT_FAILURE;
	}

	//fprintf(stdout, "sequenceName: %s\nrowNumber: %ld\nstartPosition: %ld\nendPosition: %ld\n", sequenceFilename, rowNumber, startPosition, endPosition);

	if (!initializeSequenceFiles(arguments.firstrule) ||
			!processFile(arguments) ||
			!destroyFirstRule(&arguments)) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


