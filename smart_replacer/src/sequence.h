/*
 * sequence.h
 *
 *  Created on: Oct 22, 2017
 *      Author: msixel
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <stdbool.h>
#include "arguments_types.h"

static const long SEQUENCE_INITIAL_VALUE = 0;

bool initializeSequenceFile(sequence_t* sequence);
bool initializeSequenceFiles(argument_rule_t* arg_rule);
long sequenceNewValue(sequence_t* sequence);

#endif /* SEQUENCE_H_ */
