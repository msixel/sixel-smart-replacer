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

bool parseRuleSequenceParameter(char* arg, sequence_t* sequence);
bool parseRuleParameter(char* arg, argument_rule_t* arg_rule);
bool parseParameters(int argc, char *argv[], arguments_t* arguments);
bool addNextRule(argument_rule_t* arg_rule_previous, argument_rule_t* arg_rule);
bool addRule(arguments_t* arguments, argument_rule_t* arg_rule);
bool destroyNextRule(argument_rule_t* arg_rule);
bool destroyFirstRule(arguments_t* arguments);

#endif /* ARGUMENTS_H_ */
