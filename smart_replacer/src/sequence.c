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
#include "arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * rotina para verificar se arquivo existe e inicializa-lo se necessario
 */
bool initializeSequenceFile(sequence_t* sequence) {
	FILE* sequenceFile;
	if ((F_OK == access(sequence->file, F_OK))) {
		return true;
	} else {
		if ((sequenceFile = fopen(sequence->file, "w")) == NULL) {
			fprintf(stderr, "Arquivo %s nao pode ser aberto para escrita.\n", sequence->file);
			return false;
		}
		fprintf(sequenceFile, "%ld\n", SEQUENCE_INITIAL_VALUE);
		fclose(sequenceFile);
		return true;
	}
}

/*
 * rotina para verificar se arquivos existem e inicializa-los se necessario
 */
bool initializeSequenceFiles(argument_rule_t* arg_rule) {
	if (arg_rule->sequence != NULL) {
		if (!initializeSequenceFile(arg_rule->sequence)) {
			return false;
		}
	}
	if (arg_rule->nextrule == NULL) { // fim de lista encadeada (folha)
		return true;
	} else {
		//ainda tem lista encadeada / descendo proximo nivel em busca da folha
		return initializeSequenceFiles(arg_rule->nextrule);
	}
}

/*
 * rotina para obter valor atual da sequence, incrementar,
 * persistir e retornar o valor incrementado
 */
long sequenceNewValue(sequence_t* sequence) {
	FILE* sequenceFile;
	long sequenceValue;

	if ((sequenceFile = fopen(sequence->file, "r+")) == NULL) {
		fprintf(stderr, "Arquivo %s nao pode ser aberto para leitura e escrita.\n", sequence->file);
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




