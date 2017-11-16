/*
 ============================================================================
 Name        : arguments.c
 Author      : Marcos Sixel
 Version     :
 Copyright   : RSI Informatica
 Description : Recebimento de argumentos de linha de comando
 ============================================================================
 */

#include "arguments.h"
#include "arguments_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_record_constants.h"

static const char* _SEQUENCE_FILE_EXTENSION = ".sequence";
static const char* _HEADER_RECORD_LABEL = "HEADER";
static const char* _DETAIL_RECORD_LABEL = "DETAIL";
static const char* _TRAILLER_RECORD_LABEL = "TRAILLER";

static const char* _PAR_INPUT = "-INPUT";
static const char* _PAR_OUTPUT = "-OUTPUT";
static const char* _PAR_RULE = "-RULE";
static const char* _PAR_INPUT_SHORT = "-I";
static const char* _PAR_OUTPUT_SHORT = "-O";
static const char* _PAR_RULE_SHORT = "-R";

static const char* _MESSAGE_WARN = "Aviso: ";
static const char* _MESSAGE_ERROR = "Erro:";
static const char* _MESSAGE_INVALID_FORMAT = "FORMATO INVALIDO";
static const char* _MESSAGE_INVALID_ARGUMENT = "ARGUMENTO INVALIDO";
static const char* _MESSAGE_REQUIRED_INPUT_FILE_ARGUMENT = "ARGUMENTO INPUT FILE NAO INFORMADO";
static const char* _MESSAGE_REQUIRED_OUTPUT_FILE_ARGUMENT = "ARGUMENTO OUTPUT FILE NAO INFORMADO";
static const char* _MESSAGE_REQUIRED_RULE_ARGUMENT = "NENHUM ARGUMENTO REGRA INFORMADO";

static const char* _MESSAGE_USAGE = "\nSixel Smart Replacer Tool for flat files\n\nUso: sixel_smart_replacer -i[nput] ARQUIVO_ORIGEM -o[utput] ARQUIVO_DESTINO -r[ule] LINHA,POSICAO_INICIAL-POSICAO_FINAL,VALOR [-r[ule] LINHA,POSICAO_INICIAL-POSICAO_FINAL,VALOR]...\n\nParametros:\n  ARQUIVO_ORIGEM\t- Arquivo de entrada a ser lido e convertido\n  ARQUIVO_DESTINO\t- Arquivo de saida a ser escrito ou sobrescrito\n  LINHA\t\t\t- Numero literal da linha ou um dos seguintes coringas;\n\t\t\t  HEADER - linha inicial do arquivo\n\t\t\t  DETAIL - da segunda ate a penultima linha\n\t\t\t  FOOTER - ultima linha do arquivo\n\t\t\t  CUSTOM - tipo especificado pelo usuario no formato;\n\t\t\t\t   custom:INICIAL-FINAL[.INICIAL-FINAL]=VALOR\n  POSICAO_INICIAL\t- Posicao inicial do bloco a ser substituido\n  POSICAO_FINAL\t\t- Posicao final do bloco a ser substituido (inclusive)\n  VALOR\t\t\t- Valor literal a ser substituido ou \n\t\t\t  Sequencia no formato seq:NOME_SEQUENCIA\n\n";
static const char* _TOKEN_RULE_DELIMITER = ",";
static const char* _TOKEN_RULE_POS_DELIMITER = "-";
static const char* _TOKEN_RULE_SEQUENCE_PREFIX = "seq:";
static const char* _TOKEN_RULE_CUSTOM_ROW_PREFIX = "custom:";
static const char* _TOKEN_RULE_CUSTOM_ROW_DELIMITER = "=";
static const char* _TOKEN_RULE_CUSTOM_ROW_BLOCK_CONCAT_DELIMITER = ".";


bool parseRuleValueParameter(char* arg, argument_rule_t* arg_rule) {
	//argumento valor (sequence ou literal)
	char* sequenceName;
	if ((sequenceName = strstr(arg, _TOKEN_RULE_SEQUENCE_PREFIX))!=NULL) {
		//criando sequence
		sequenceName += strlen(_TOKEN_RULE_SEQUENCE_PREFIX);//avancando o token para obter o nome da sequence
		arg_rule->sequence = (sequence_t*)malloc(sizeof(sequence_t));
		arg_rule->sequence->name = strdup(sequenceName);
		arg_rule->sequence->file = (char*) calloc(strlen (sequenceName) + strlen(_SEQUENCE_FILE_EXTENSION) + 1, sizeof(char));// alocando memoria dinamicamente do heap
		arg_rule->sequence->file = strcpy(arg_rule->sequence->file, sequenceName);
		arg_rule->sequence->file = strcat(arg_rule->sequence->file, _SEQUENCE_FILE_EXTENSION);

		arg_rule->literalValue=NULL;
	} else {
		arg_rule->sequence = NULL;
		arg_rule->literalValue = strdup(arg);
	}
	return true;
}

bool parseRuleRowCustomBlockParameter(char* arg, custom_row_block_t* custom_row_block) {
	//8-8
	char* argSubToken;

	if (strlen(arg)==0 || (argSubToken = strtok_r(arg, _TOKEN_RULE_POS_DELIMITER, &arg))==NULL) {
		fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_INVALID_FORMAT, arg);
		return false;
	}
	custom_row_block->startPosition = strtol(argSubToken, NULL, 0);

	if ((argSubToken = strtok_r(arg, _TOKEN_RULE_POS_DELIMITER, &arg))==NULL) {
		fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_INVALID_FORMAT, arg);
		return false;
	}
	custom_row_block->endPosition = strtol(argSubToken, NULL, 0);
	custom_row_block->nextblock = NULL;
	//validando sequence
	if (custom_row_block->startPosition < 1) {
		fprintf (stderr, "%s\nPOSICAO_INICIAL INVALIDA [%ld]\n", _MESSAGE_USAGE, custom_row_block->startPosition);
		return false;
	}
	if (custom_row_block->startPosition > custom_row_block->endPosition) {
		fprintf (stderr, "%s\nPOSICAO_FINAL DEVE SER MAIOR QUE POSICAO_INICIAL [%ld, %ld]\n", _MESSAGE_USAGE, custom_row_block->startPosition, custom_row_block->endPosition);
		return false;
	}

	return true;
}

bool parseRuleRowCustomBlockParameters(char* arg, custom_row_t* custom_row) {
	// 8-8.14-14.73-77
	char* argToken;
	custom_row_block_t* custom_row_block;

	argToken = strtok_r(arg, _TOKEN_RULE_CUSTOM_ROW_BLOCK_CONCAT_DELIMITER, &arg);
	while (argToken != NULL) {
		//argumento blocos posicionais - lista encadeada   - 8-8.14-14
		if (strlen(argToken)==0) {
			fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_INVALID_FORMAT, argToken);
			return false;
		}
		custom_row_block = malloc(sizeof(custom_row_block_t));
		if (!parseRuleRowCustomBlockParameter(argToken, custom_row_block)) {
			return false;
		}
		//adicionando no final da lista encadeada
		appendRuleCustomRowBlock(custom_row, custom_row_block);

		argToken = strtok_r(arg, _TOKEN_RULE_CUSTOM_ROW_BLOCK_CONCAT_DELIMITER, &arg);
	}
	return true;
}


bool parseRuleRowCustomParameter(char* arg, custom_row_t* custom_row ) {
	int contador;
	char* argToken;

	custom_row->firstblock = NULL;
	custom_row->discriminator = NULL;

	argToken = strtok_r(arg, _TOKEN_RULE_CUSTOM_ROW_DELIMITER, &arg);
	for (contador=0; argToken != NULL; contador++ ) {
		switch(contador) {
		case 0:
			//argumento blocos posicionais - lista encadeada   - 8-8.14-14
			if (strlen(argToken)==0) {
				fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_INVALID_FORMAT, argToken);
				return false;
			}
//			custom_row->firstblock = malloc(sizeof(custom_row_block_t));
			if (!parseRuleRowCustomBlockParameters(argToken, custom_row)) {
				return false;
			}
			break;

		case 1:
			//argumento valor discriminador/identificador
			custom_row->discriminator = strdup(argToken);
			break;

		default:
			fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_WARN, _MESSAGE_INVALID_FORMAT, arg);
			break;
		}
		argToken = strtok_r(arg, _TOKEN_RULE_DELIMITER, &arg);
	}
	return true;
}

bool parseRuleRowParameter(char* arg, argument_rule_t* arg_rule) {
	char* customRowArg;

	if (strcasecmp(_HEADER_RECORD_LABEL, arg)==0) {
		arg_rule->custom_row = NULL;
		arg_rule->rowNumber = _HEADER_RECORD_CODE;

	} else if (strcasecmp(_DETAIL_RECORD_LABEL, arg)==0 ) {
		arg_rule->custom_row = NULL;
		arg_rule->rowNumber = _DETAIL_RECORD_CODE;

	} else if (strcasecmp(_TRAILLER_RECORD_LABEL, arg)==0) {
		arg_rule->custom_row = NULL;
		arg_rule->rowNumber = _TRAILLER_RECORD_CODE;

	} else if ((customRowArg = strstr(arg, _TOKEN_RULE_CUSTOM_ROW_PREFIX))!=NULL) {
		customRowArg += strlen(_TOKEN_RULE_CUSTOM_ROW_PREFIX);//avancando o token para obter as definicoes do custom row
		arg_rule->custom_row = (custom_row_t*) malloc(sizeof(custom_row_t));
		arg_rule->rowNumber = _CUSTOM_RECORD_CODE;
		if (!parseRuleRowCustomParameter(customRowArg, arg_rule->custom_row)) {
			return false;
		}
	} else {
		arg_rule->custom_row = NULL;
		arg_rule->rowNumber = strtol(arg, NULL, 0);
	}

	return true;
}

bool parseRuleParameter(char* arg, argument_rule_t* arg_rule){
	int contador;
	char* argToken;
	char* argPositionsToken;

	argToken = strtok_r(arg, _TOKEN_RULE_DELIMITER, &arg);

	for (contador=0; argToken != NULL; contador++ ) {
		switch(contador) {
		case 0:
			//argumento linha
			if (!parseRuleRowParameter(argToken, arg_rule)) {
				return false;
			}
			break;

		case 1:
			//argumento posicao inicial e posicao final
			argPositionsToken = strtok_r(argToken, _TOKEN_RULE_POS_DELIMITER, &argToken);
			if (argPositionsToken == NULL) {
				fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_INVALID_FORMAT, argToken);
				return false;
			}
			arg_rule->startPosition = strtol(argPositionsToken, NULL, 0);
			argPositionsToken = strtok_r(argToken, _TOKEN_RULE_POS_DELIMITER, &argToken);
			if (argPositionsToken == NULL) {
				fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_INVALID_FORMAT, argToken);
				return false;
			}
			arg_rule->endPosition = strtol(argPositionsToken, NULL, 0);
			argPositionsToken = strtok_r(argToken, _TOKEN_RULE_POS_DELIMITER, &argToken);
			if (argPositionsToken != NULL) {
				fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_ERROR, _MESSAGE_INVALID_FORMAT, argToken);
				return false;
			}
			break;

		case 2:
			//argumento valor (sequence ou literal)
			if (!parseRuleValueParameter(argToken, arg_rule)) {
				return false;
			}
			break;

		default:
			fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_WARN, _MESSAGE_INVALID_FORMAT, arg);
			break;
		}
		argToken = strtok_r(arg, _TOKEN_RULE_DELIMITER, &arg);
	}

	arg_rule->nextrule = NULL;
	//validando sequence
	if (arg_rule->startPosition < 1) {
		fprintf (stderr, "%s\nPOSICAO_INICIAL INVALIDA [%ld]\n", _MESSAGE_USAGE, arg_rule->startPosition);
		return false;
	}
	if (arg_rule->startPosition > arg_rule->endPosition) {
		fprintf (stderr, "%s\nPOSICAO_FINAL DEVE SER MAIOR QUE POSICAO_INICIAL [%ld, %ld]\n", _MESSAGE_USAGE, arg_rule->startPosition, arg_rule->endPosition);
		return false;
	}

	return true;
}

bool parseArgumentsParameters(int argc, char *argv[], arguments_t* arguments) {
	int contador;
	bool usageMessagePrinted = false;

	arguments->inputfile = NULL;
	arguments->outputfile = NULL;
	arguments->firstrule = NULL;

	for (contador=1; contador<argc; contador++) {
		char* arg = argv[contador];

		if (strlen(arg)==0 || arg[0]!='-') {
			if (!usageMessagePrinted) {
				fprintf(stderr, "%s\n", _MESSAGE_USAGE);
				usageMessagePrinted = true;
			}
			fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_WARN, _MESSAGE_INVALID_FORMAT, arg);
		} else {
			if (strcasecmp(arg, _PAR_INPUT)==0 ||
					strcasecmp(arg, _PAR_INPUT_SHORT)==0) {
				contador++;
				if (contador<argc) {
					arguments->inputfile = strdup(argv[contador]);
				}
			} else if (strcasecmp(arg, _PAR_OUTPUT)==0 ||
					strcasecmp(arg, _PAR_OUTPUT_SHORT)==0) {
				contador++;
				if (contador<argc) {
					arguments->outputfile = strdup(argv[contador]);
				}
			} else if (strcasecmp(arg, _PAR_RULE)==0 ||
					strcasecmp(arg, _PAR_RULE_SHORT)==0) {
				contador++;
				if (contador<argc) {
					argument_rule_t* arg_rule;
					arg_rule = (argument_rule_t*) malloc(sizeof(argument_rule_t));
					if (!parseRuleParameter(argv[contador], arg_rule)) {
						return false;
					}
					appendRule(arguments, arg_rule);
				}
			} else {
				if (!usageMessagePrinted) {
					fprintf(stderr, "%s\n", _MESSAGE_USAGE);
					usageMessagePrinted = true;
				}
				fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_WARN, _MESSAGE_INVALID_ARGUMENT, arg);
			}
		}
	}

	if (arguments->inputfile == NULL) {
		if (!usageMessagePrinted) {
			fprintf(stderr, "%s\n", _MESSAGE_USAGE);
			usageMessagePrinted = true;
		}
		fprintf (stderr, "%s\t%s\n", _MESSAGE_ERROR, _MESSAGE_REQUIRED_INPUT_FILE_ARGUMENT);
		return false;
	}

	if (arguments->outputfile == NULL) {
		if (!usageMessagePrinted) {
			fprintf(stderr, "%s\n", _MESSAGE_USAGE);
			usageMessagePrinted = true;
		}
		fprintf (stderr, "%s\t%s\n", _MESSAGE_ERROR, _MESSAGE_REQUIRED_OUTPUT_FILE_ARGUMENT);
		return false;
	}

	if (arguments->firstrule == NULL) {
		if (!usageMessagePrinted) {
			fprintf(stderr, "%s\n", _MESSAGE_USAGE);
			usageMessagePrinted = true;
		}
		fprintf (stderr, "%s\t%s\n", _MESSAGE_ERROR, _MESSAGE_REQUIRED_RULE_ARGUMENT);
		return false;
	}

	return true;
}

bool appendNextRuleCustomRowBlock(custom_row_block_t* custom_row_block_previous, custom_row_block_t* custom_row_block) {
	//adicionando custom row block no fim da lista encadeada
	if (custom_row_block_previous->nextblock == NULL) {
		custom_row_block_previous->nextblock = custom_row_block;
		return true;
	} else {
		return appendNextRuleCustomRowBlock(custom_row_block_previous->nextblock, custom_row_block);
	}
}

bool appendRuleCustomRowBlock(custom_row_t* custom_row, custom_row_block_t* custom_row_block){
	//adicionando custom row block no fim da lista encadeada
	if (custom_row->firstblock == NULL) {
		custom_row->firstblock = custom_row_block;
		return true;
	} else {
		return appendNextRuleCustomRowBlock(custom_row->firstblock, custom_row_block);
	}
}

bool appendNextRule(argument_rule_t* arg_rule_previous, argument_rule_t* arg_rule) {
	//adicionando rule no fim da lista encadeada
	if (arg_rule_previous->nextrule == NULL) {
		arg_rule_previous->nextrule = arg_rule;
		return true;
	} else {
		return appendNextRule(arg_rule_previous->nextrule, arg_rule);
	}
}

bool appendRule(arguments_t* arguments, argument_rule_t* arg_rule){
	//adicionando rule no fim da lista encadeada
	if (arguments->firstrule == NULL) {
		arguments->firstrule = arg_rule;
		return true;
	} else {
		return appendNextRule(arguments->firstrule, arg_rule);
	}
}

void destroyRuleCustomRowBlock(custom_row_block_t* custom_row_block) {
	if (custom_row_block->nextblock != NULL) {
		destroyRuleCustomRowBlock(custom_row_block->nextblock);
		custom_row_block->nextblock = NULL;
	}
	free(custom_row_block);
}

void destroyRuleCustomRow(custom_row_t* custom_row) {
	if (custom_row->firstblock != NULL) {
		destroyRuleCustomRowBlock(custom_row->firstblock);
		custom_row->firstblock = NULL;
	}
	free(custom_row->discriminator);
	custom_row->discriminator = NULL;
}

void destroyRule(argument_rule_t* arg_rule) {
	if (arg_rule->custom_row != NULL) {
		destroyRuleCustomRow(arg_rule->custom_row);
		free(arg_rule->custom_row);
		arg_rule->custom_row = NULL;
	}
	if (arg_rule->nextrule != NULL) {
		destroyRule(arg_rule->nextrule);
		arg_rule->nextrule = NULL;
	}
	if (arg_rule->sequence != NULL) {
		free(arg_rule->sequence->file);
		arg_rule->sequence->file = NULL;
		free(arg_rule->sequence->name);
		arg_rule->sequence->name = NULL;
		free(arg_rule->sequence);
		arg_rule->sequence = NULL;
	}
	if (arg_rule->literalValue != NULL) {
		free(arg_rule->literalValue);
		arg_rule->literalValue = NULL;
	}
	free(arg_rule);
}

void destroyArguments(arguments_t* arguments) {
	if (arguments->firstrule != NULL) {
		destroyRule(arguments->firstrule);
		arguments->firstrule = NULL;
	}
	free(arguments->inputfile);
	free(arguments->outputfile);
}

