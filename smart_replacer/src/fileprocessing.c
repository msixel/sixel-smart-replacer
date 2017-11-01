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
#include "file_record_constants.h"

bool processRowValue (char* rowValue, int rowNumber, argument_rule_t* arg_rule) {
	int blockLength;
	char* blockBuffer;

	if (arg_rule->rowNumber == rowNumber /* pega linhas definidas de forma literal, header e trailler(-9)*/ ||
			(arg_rule->rowNumber == _DETAIL_RECORD_CODE && rowNumber>1 /* pega linhas de detalhe */ )) {
		//localizada linha para receber tratamento

		if (arg_rule->endPosition > strlen(rowValue)) {
			fprintf(stderr, "BLOCO NAO LOCALIZADO NA LINHA [POSICAO_INICIAL: %ld, POSICAO_FINAL: %ld]",
					arg_rule->startPosition, arg_rule->endPosition);
			return false;
		}

		if (arg_rule->sequence != NULL) {
			long sequenceValue;
			if ((sequenceValue = sequenceNewValue(arg_rule->sequence)) == -1)
				return false;

			//criando novo bloco
			blockLength = (arg_rule->endPosition - arg_rule->startPosition + 1);
			blockBuffer = (char*) calloc(blockLength, sizeof(char));
			sprintf(blockBuffer, "%0*ld", blockLength, sequenceValue);
			memcpy(rowValue + ((arg_rule->startPosition - 1) * sizeof(char)), blockBuffer, blockLength * sizeof(char));
			free(blockBuffer);

		} else if (arg_rule->literalValue != NULL) {
			blockLength = strlen(arg_rule->literalValue);
			if (arg_rule->endPosition - arg_rule->startPosition + 1 != blockLength ) {
				fprintf(stderr, "BLOCO PARA SUBSTITUICAO DE TAMANHO INVALIDO [POSICAO_INICIAL: %ld, POSICAO_FINAL: %ld, BLOCK_LENGTH: %ld]",
						arg_rule->startPosition, arg_rule->endPosition, blockLength);
				return false;
			}
			memcpy(rowValue + ((arg_rule->startPosition - 1) * sizeof(char)), arg_rule->literalValue, (blockLength) * sizeof(char));

		} else {
			fprintf(stderr, "ESPERADO SEQUENCE OU VALOR LITERAL");
			return false;
		}
	}

	if (arg_rule->nextrule != NULL) {
		return processRowValue(rowValue, rowNumber, arg_rule->nextrule);
	}

	return true;
}


bool processFile(arguments_t arguments) {
	FILE* inputFile;
	FILE* outputFile;
	char rowValueBuffer[2][1024];
	int currentRowBuffer;
	int previousRowBuffer;
	int rowNumber;

	//abrindo arquivos
	if ((inputFile = fopen(arguments.inputfile, "r")) == NULL) {
		fprintf(stderr, "Arquivo %s nao pode ser aberto para leitura.\n", arguments.inputfile);
		return false;
	}
	if ((outputFile = fopen(arguments.outputfile, "w")) == NULL) {
		fprintf(stderr, "Arquivo %s nao pode ser aberto para escrita.\n", arguments.outputfile);
		return false;
	}

	currentRowBuffer = 0;
	previousRowBuffer = -1;
	//iterando arquivos e fazendo substituicoes
	for (rowNumber=0; fscanf(inputFile, "%s\n", rowValueBuffer[currentRowBuffer])!= EOF; rowNumber++) {
		if (previousRowBuffer>-1) {
			//hora da escrita atrasada no arquivo de saida - precisa disso para identificar registro trailler
			if (!processRowValue(rowValueBuffer[previousRowBuffer], rowNumber, arguments.firstrule)) {
				fclose(inputFile);
				fclose(outputFile);
				return false;
			}
			fprintf(outputFile, "%s\n", rowValueBuffer[previousRowBuffer]);
		}
		//swap entre buffers
		previousRowBuffer = currentRowBuffer;
		currentRowBuffer = (currentRowBuffer==0)?1:0;
	}

	if (previousRowBuffer>-1) {
		//hora da escrita atrasada no arquivo de saida - precisa disso para identificar registro trailler
		if (!processRowValue(rowValueBuffer[previousRowBuffer], _TRAILLER_RECORD_CODE, arguments.firstrule)) {
			fclose(inputFile);
			fclose(outputFile);
			return false;
		}
		fprintf(outputFile, "%s\n", rowValueBuffer[previousRowBuffer]);
	}

	//fechando arquivos
	fclose(inputFile);
	fclose(outputFile);

	return true;
}
