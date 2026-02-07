//
// Created by davi on 2/5/26.
//

#ifndef MY_OWN_EMULATOR_ERROR_H
#define MY_OWN_EMULATOR_ERROR_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void setErrorContext(uint16_t line, const char* lexicalElement, const char* errorType);

void printError();

#endif //MY_OWN_EMULATOR_ERROR_H