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
bool parseRuleValueParameter(char* arg, argument_rule_t* arg_rule);
bool parseRuleRowCustomParameter(char* arg, custom_row_t* custom_row);
bool parseRuleRowParameter(char* arg, argument_rule_t* arg_rule);
bool parseRuleParameter(char* arg, argument_rule_t* arg_rule);
bool parseArgumentsParameters(int argc, char *argv[], arguments_t* arguments);
bool appendNextRule(argument_rule_t* arg_rule_previous, argument_rule_t* arg_rule);
bool appendRule(arguments_t* arguments, argument_rule_t* arg_rule);
void destroyRuleRowCustomBlock(custom_row_block* custom_row_block);
void destroyRule(argument_rule_t* arg_rule);
void destroyArguments(arguments_t* arguments);

#endif /* ARGUMENTS_H_ */
