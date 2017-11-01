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

#define _BUFFER_LINE_SIZE 1024

static const char* _MESSAGE_ERROR = "Erro:";
static const char* _MESSAGE_INVALID_RANGE = "BLOCO NAO LOCALIZADO NA LINHA";
static const char* _MESSAGE_INVALID_LITERAL_LENGHT = "BLOCO PARA SUBSTITUICAO DE TAMANHO INVALIDO";
static const char* _MESSAGE_REPLACEMENT_NOT_FOUND = "ESPERADO SEQUENCE OU VALOR LITERAL";

static const char* _MESSAGE_FILE_NOT_OPEN_READ = "ARQUIVO NAO PODE SER ABERTO PARA LEITURA";
static const char* _MESSAGE_FILE_NOT_OPEN_WRITE = "ARQUIVO NAO PODE SER ABERTO PARA ESCRITA";

bool processRowValue (char* rowValue, int rowNumber, argument_rule_t* arg_rule) {
	int blockLength;
	char* blockBuffer;

	if (arg_rule->rowNumber == rowNumber /* pega linhas definidas de forma literal, header e trailler(-9)*/ ||
			(arg_rule->rowNumber == _DETAIL_RECORD_CODE && rowNumber>1 /* pega linhas de detalhe */ )) {
		//localizada linha para receber tratamento

		if (arg_rule->endPosition > strlen(rowValue)) {
			fprintf(stderr, "%s\t%s [POSICAO_INICIAL: %ld, POSICAO_FINAL: %ld, TAMANHO: %ld %s]\n",
					_MESSAGE_ERROR, _MESSAGE_INVALID_RANGE, arg_rule->startPosition, arg_rule->endPosition, strlen(rowValue), rowValue);
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
				fprintf(stderr, "%s\t%s [POSICAO_INICIAL: %ld, POSICAO_FINAL: %ld, BLOCK_LENGTH: %d]\n",
						_MESSAGE_ERROR, _MESSAGE_INVALID_LITERAL_LENGHT, arg_rule->startPosition, arg_rule->endPosition, blockLength);
				return false;
			}
			memcpy(rowValue + ((arg_rule->startPosition - 1) * sizeof(char)), arg_rule->literalValue, (blockLength) * sizeof(char));

		} else {
			fprintf(stderr, "%s\t%s\n", _MESSAGE_ERROR, _MESSAGE_REPLACEMENT_NOT_FOUND);
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
	char rowValueBuffer[2][_BUFFER_LINE_SIZE];
	int currentRowBuffer;
	int previousRowBuffer;
	int rowNumber;

	//abrindo arquivos
	if ((inputFile = fopen(arguments.inputfile, "r")) == NULL) {
		fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_FILE_NOT_OPEN_READ, arguments.inputfile);
		return false;
	}
	if ((outputFile = fopen(arguments.outputfile, "w")) == NULL) {
		fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_FILE_NOT_OPEN_WRITE, arguments.outputfile);
		return false;
	}

	currentRowBuffer = 0;
	previousRowBuffer = -1;
	//iterando arquivos e fazendo substituicoes
	for (rowNumber=0; fgets(rowValueBuffer[currentRowBuffer], _BUFFER_LINE_SIZE, inputFile); rowNumber++) {
		if (previousRowBuffer>-1) {
			//hora da escrita atrasada no arquivo de saida - precisa disso para identificar registro trailler
			if (!processRowValue(rowValueBuffer[previousRowBuffer], rowNumber, arguments.firstrule)) {
				fclose(inputFile);
				fclose(outputFile);
				return false;
			}
			fprintf(outputFile, "%s", rowValueBuffer[previousRowBuffer]);
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
