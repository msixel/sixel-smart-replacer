/*
 * fileprocessing.h
 *
 *  Created on: Oct 22, 2017
 *      Author: msixel
 */

#ifndef ARGUMENTS_TYPES_H_
#define ARGUMENTS_TYPES_H_

typedef struct sequence {
	char* name;
	char* file;
} sequence_t;

typedef struct argument_rule {
	struct sequence* sequence;
	char* literalValue;
	long rowNumber;
	long startPosition;
	long endPosition;
	struct argument_rule* nextrule;
} argument_rule_t;

typedef struct arguments {
	char* inputfile;
	char* outputfile;
	struct argument_rule* firstrule;
} arguments_t;

#endif /* ARGUMENTS_TYPES_H_ */
