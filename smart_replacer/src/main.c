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
	arguments_t arguments;

	if (!parseArgumentsParameters(argc, argv, &arguments)) {
		return EXIT_FAILURE;
	}

	if (!initializeSequenceFiles(arguments.firstrule) ||
			!processFile(arguments) ||
			!destroyArguments(&arguments)) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


