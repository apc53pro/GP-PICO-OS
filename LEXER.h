#include "draw_funcs.h"
#include "globals.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "PS2_KB.h"
#include "pico/stdlib.h"
#include "pico/platform.h"
#include "Arduino.h"
#include "func_exports.h"

#ifndef LEXER_H
#define LEXER_H

int precedenceCheck(char op);

int minZero(int n);

double mathsSolver(String expr);

String ARGUMENT_STRUCTURE_ANALYZER(String arg, String argStructure, int returnContentField);

bool isValidSymbol(char ch);

void clearVBuffer(uint8_t color);

double PRINT(String code);

int variableFormatValidator(String varName);

int8_t varTypeLUT(String varType);

int8_t LEXER(String code);

#endif