/*
 * sequence.h
 *
 *  Created on: Oct 22, 2017
 *      Author: msixel
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

#include <stdbool.h>

bool initializeSequenceFile(char* sequenceFilename);
long sequenceNewValue(char* sequenceFilename);

#endif /* SEQUENCE_H_ */
