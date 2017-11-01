/*
 * fileprocessing.h
 *
 *  Created on: Oct 22, 2017
 *      Author: msixel
 */

#ifndef FILEPROCESSING_H_
#define FILEPROCESSING_H_

#include <stdbool.h>
#include "arguments_types.h"

bool processRowValue (char* rowValue, int rowNumber, argument_rule_t* argument_rule);
bool processFile(arguments_t arguments);

#endif /* FILEPROCESSING_H_ */
