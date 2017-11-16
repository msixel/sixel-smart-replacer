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

bool parseRuleValueParameter(char* arg, argument_rule_t* arg_rule);
bool parseRuleRowCustomBlockParameter(char* arg, custom_row_block_t* custom_row_block);
bool parseRuleRowCustomBlockParameters(char* arg, custom_row_t* custom_row);
bool parseRuleRowCustomParameter(char* arg, custom_row_t* custom_row);
bool parseRuleRowParameter(char* arg, argument_rule_t* arg_rule);
bool parseRuleParameter(char* arg, argument_rule_t* arg_rule);
bool parseArgumentsParameters(int argc, char *argv[], arguments_t* arguments);

bool appendNextRuleCustomRowBlock(custom_row_block_t* custom_row_block_previous, custom_row_block_t* custom_row_block);
bool appendRuleCustomRowBlock(custom_row_t* custom_row, custom_row_block_t* custom_row_block);

bool appendNextRule(argument_rule_t* arg_rule_previous, argument_rule_t* arg_rule);
bool appendRule(arguments_t* arguments, argument_rule_t* arg_rule);

void destroyRuleCustomRowBlock(custom_row_block_t* custom_row_block);
void destroyRuleCustomRow(custom_row_t* custom_row);
void destroyRule(argument_rule_t* arg_rule);
void destroyArguments(arguments_t* arguments);

#endif /* ARGUMENTS_H_ */
