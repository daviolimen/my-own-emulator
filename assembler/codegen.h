//
// Created by davi on 2/5/26.
//

#ifndef MY_OWN_EMULATOR_CODEGEN_H
#define MY_OWN_EMULATOR_CODEGEN_H
#include <stdio.h>
#include <stdint.h>
#include "error.h"
#include "globals.h"

// Struct to store labels
typedef struct {
    char label[32];
    uint16_t address;
} label;

extern label labels[128];
extern int labelCnt;

// Function to find label in the array, O(N) for simplicity, could be optimized to hash for O(1) or RBT for O(log N)
int findLabel(const char* name);

// Function to add label to the label array
void addLabel(const char* name, uint16_t address);

extern uint8_t dataSection[];

// Function to process assembler directives
int processDirective(const char* directive, const char* line);

static const char* registersStrings[] = {
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
    "[R0]", "[R1]", "[R2]", "[R3]", "[R4]", "[R5]", "[R6]", "[R7]"
};

int checkRegister(const char* reg);

int genCode(FILE* outputFile, uint8_t opcode, const char* op1, const char* op2, const char* op3);

int genLoad(FILE* outputFile, uint8_t opcode, const char* reg, uint8_t imm);

int genMov(FILE* outputFile, uint8_t opcode, const char* reg1, const char* reg2);

int genArit(FILE* outputFile, uint8_t opcode, const char* reg1, const char* reg2, const char* reg3);

int genJumpPushPop(FILE* outputFile, uint8_t opcode, const char* operand);

int genOthers(FILE* outputFile, uint8_t opcode);

#endif //MY_OWN_EMULATOR_CODEGEN_H