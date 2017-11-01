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

static const char* _MESSAGE_WARN = "Aviso: ";
static const char* _MESSAGE_ERROR = "Erro:";
static const char* _MESSAGE_INVALID_FORMAT = "FORMATO INVALIDO";
static const char* _MESSAGE_INVALID_ARGUMENT = "ARGUMENTO INVALIDO";
static const char* _MESSAGE_REQUIRED_INPUT_FILE_ARGUMENT = "ARGUMENTO INPUT FILE NAO INFORMADO";
static const char* _MESSAGE_REQUIRED_OUTPUT_FILE_ARGUMENT = "ARGUMENTO OUTPUT FILE NAO INFORMADO";
static const char* _MESSAGE_REQUIRED_RULE_ARGUMENT = "NENHUM ARGUMENTO REGRA INFORMADO";

static const char* _MESSAGE_USAGE = "Uso: sixel_smart_replacer -input ARQUIVO_ORIGEM -output ARQUIVO_DESTINO -rule LINHA,POSICAO_INICIAL-POSICAO_FINAL,VALOR [-rule LINHA,POSICAO_INICIAL-POSICAO_FINAL,VALOR]...\n\nParametros:\n\tARQUIVO_ORIGEM - Arquivo a ser lido e convertido\n\tARQUIVO_DESTINO - Arquivo a ser escrito ou sobrescrito\n\tPOSICAO_INICIAL - Posicao inicial a ser substituida\n\tPOSICAO_FINAL - Posicao final a ser substituida (inclusive)\n\tVALOR - Valor literal ou sequencia a ser escrito (sequencia no formato seq:NOME_SEQUENCIA)\n\n";
static const char* _TOKEN_RULE_DELIMITER = ",";
static const char* _TOKEN_RULE_POS_DELIMITER = "-";
static const char* _TOKEN_RULE_SEQUENCE_PREFIX = "seq:";


bool parseRuleSequenceParameter(char* arg, sequence_t* sequence) {
	sequence->name = strdup(arg);
	sequence->file = (char*) calloc(strlen (arg) + strlen(_SEQUENCE_FILE_EXTENSION) + 1, sizeof(char));// alocando memoria dinamicamente do heap
	sequence->file = strcpy(sequence->file, arg);
	sequence->file = strcat(sequence->file, _SEQUENCE_FILE_EXTENSION);
	return true;
}

bool parseRuleValueParameter(char* arg, argument_rule_t* arg_rule) {
	//argumento valor (sequence ou literal)
	char* sequenceName;
	if ((sequenceName = strstr(arg, _TOKEN_RULE_SEQUENCE_PREFIX))!=NULL) {
		//criando sequence
		sequenceName += strlen(_TOKEN_RULE_SEQUENCE_PREFIX);//avancando o token para obter o nome da sequence
		arg_rule->sequence = (sequence_t*)malloc(sizeof(sequence_t));
		if (!parseRuleSequenceParameter(sequenceName, arg_rule->sequence)) {
			return false;
		}
		arg_rule->literalValue=NULL;
	} else {
		arg_rule->sequence = NULL;
		arg_rule->literalValue = strdup(arg);
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
			if (strcasecmp(_HEADER_RECORD_LABEL, argToken)==0) {
				arg_rule->rowNumber = _HEADER_RECORD_CODE;

			} else if (strcasecmp(_DETAIL_RECORD_LABEL, argToken)==0 ) {
				arg_rule->rowNumber = _DETAIL_RECORD_CODE;

			} else if (strcasecmp(_TRAILLER_RECORD_LABEL, argToken)==0) {
				arg_rule->rowNumber = _TRAILLER_RECORD_CODE;

			} else {
				arg_rule->rowNumber = strtol(argToken, NULL, 0);
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
				fprintf(stderr, "%s\t%s [%s]\n", _MESSAGE_WARN, _MESSAGE_INVALID_FORMAT, argToken);
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
		fprintf (stderr, "POSICAO_INICIAL INVALIDA [%ld]\n", arg_rule->startPosition);
		return false;
	}
	if (arg_rule->startPosition > arg_rule->endPosition) {
		fprintf (stderr, "POSICAO_FINAL DEVE SER MAIOR QUE POSICAO_INICIAL [%ld, %ld]\n", arg_rule->startPosition, arg_rule->endPosition);
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
			if (strcasecmp(arg, _PAR_INPUT)==0) {
				contador++;
				if (contador<argc) {
					arguments->inputfile = strdup(argv[contador]);
				}
			} else if (strcasecmp(arg, _PAR_OUTPUT)==0) {
				contador++;
				if (contador<argc) {
					arguments->outputfile = strdup(argv[contador]);
				}
			} else if (strcasecmp(arg, _PAR_RULE)==0) {
				contador++;
				if (contador<argc) {
					argument_rule_t* arg_rule;
					arg_rule = (argument_rule_t*) malloc(sizeof(argument_rule_t));
					parseRuleParameter(argv[contador], arg_rule);
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


bool destroyRule(argument_rule_t* arg_rule) {
	if (arg_rule->nextrule != NULL) {
		destroyRule(arg_rule->nextrule);
		arg_rule->nextrule = NULL;
	}
	if (arg_rule->sequence != NULL) {
		free(arg_rule->sequence->file);
		free(arg_rule->sequence->name);
		free(arg_rule->sequence);
	}
	if (arg_rule->literalValue != NULL) {
		free(arg_rule->literalValue);
	}
	free(arg_rule);
	return true;
}

bool destroyArguments(arguments_t* arguments) {
	if (arguments->firstrule != NULL) {
		destroyRule(arguments->firstrule);
		arguments->firstrule = NULL;
	}
	free(arguments->inputfile);
	free(arguments->outputfile);
//	free(arguments); //alocacao estatica com variavel typedef - nao utilizou malloc, calloc ou realloc
	return true;
}

