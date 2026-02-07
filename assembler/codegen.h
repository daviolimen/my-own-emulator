//
// Created by davi on 2/5/26.
//

#ifndef MY_OWN_EMULATOR_CODEGEN_H
#define MY_OWN_EMULATOR_CODEGEN_H
#include "error.h"
#include "globals.h"


// Function to find label in the array, O(N) for simplicity, could be optimized to hash for O(1) or RBT for O(log N)
int findLabel(const char* name);

// Function to add label to the label array
void addLabel(const char* name, uint16_t address);

extern uint8_t dataSection[];

// Function to process assembler directives
int processDirective(const char* directive, const char* line);

int genCode(FILE* outputFile, uint8_t opcode, const char* op1, const char* op2, const char* op3);

#endif //MY_OWN_EMULATOR_CODEGEN_H