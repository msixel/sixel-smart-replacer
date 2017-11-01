/*
 * fileprocessing.h
 *
 *  Created on: Oct 22, 2017
 *      Author: msixel
 */

#ifndef FILEPROCESSING_H_
#define FILEPROCESSING_H_

#include <stdbool.h>

bool replaceSequenceValue (char* rowValue, char* sequenceFilename, int startPosition, int endPosition);
bool processFile(char* sourceFilename, char* sequenceFilename, int rowNumber, int startPosition, int endPosition, char* targetFilename);

#endif /* FILEPROCESSING_H_ */
