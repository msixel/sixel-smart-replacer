/*
 * fileprocessing.h
 *
 *  Created on: Oct 22, 2017
 *      Author: msixel
 */

#ifndef ARGUMENTS_H_
#define ARGUMENTS_H_

#include <stdbool.h>
#include "arguments_types.h"

bool receiveSequenceParameter(char* arg, sequence_t* sequence);
bool receiveRuleParameter(char* arg, argument_rule_t* arg_rule);
bool receiveParameters(int argc, char *argv[], arguments_t* arguments);
bool addRule(arguments_t* arguments, argument_rule_t* arg_rule);
bool destroyRules(arguments_t* arguments);

#endif /* ARGUMENTS_H_ */
