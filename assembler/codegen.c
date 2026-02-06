//
// Created by davi on 2/5/26.
//

#include "codegen.h"

#include <stdlib.h>

// Array for the labels
label labels[128];
int labelCnt = 0;

// Function to find label in the array, O(N) for simplicity, could be optimized to hash for O(1) or RBT for O(log N)
int findLabel(const char* name) {
    for (int i = 0; i < 128; i++) {
        if (strcmp(labels[i].label, name) == 0) return labels[i].address;
    }
    return -1;
}

// Function to add label to the label array
void addLabel(const char* name, uint16_t address) {
    strcpy(labels[labelCnt].label, name);
    labels[labelCnt].address = address;
    labelCnt++;
}

// Function to check if a register operand is valid and return its value
int checkRegister(const char* reg) {
    for (uint8_t i = 0; i < 16; i++) {
        if (strcmp(reg, registersStrings[i]) == 0) return i > 7 ? i + 8 : i;
    }
    return -1;
}

// Function to generate the instructions, there are faster ways than using lots of if-statements, like using an array
// of function addresses, as in the emulator, but there is no need to complicate things here
int genCode(FILE* outputFile, uint8_t opcode, const char* op1, const char* op2, const char* op3) {
    if (opcode < 2) {
        uint8_t imm = strtol(op2, NULL, 0);
        return genLoad(outputFile, opcode, op1, imm);
    }
    if (opcode == 2) return genMov(outputFile, opcode, op1, op2);
    if (opcode < 12) return genArit(outputFile, opcode, op1, op2, op3);
    if (opcode < 19) return genJumpPushPop(outputFile, opcode, op1);
    if (opcode < 23) return genOthers(outputFile, opcode);

    uint16_t imm;
    if ((op2[0] >= '0') && (op2[0] <= '9')) imm = strtol(op2, NULL, 0);
    else {
        imm = findLabel(op2);
        if (imm == (uint16_t) -1) {
            setErrorContext(lineCnt, op2, "Invalid label");
            return -1;
        }
    }
    return genLoad(outputFile, 0, op1, imm & 0xFF) | genLoad(outputFile, 1, op1, imm >> 8);
}

int genLoad(FILE* outputFile, uint8_t opcode, const char* reg, uint8_t imm) {
    int regNum = checkRegister(reg);
    if (regNum == -1 || regNum > 7) {
        setErrorContext(lineCnt, reg, "Invalid operand");
        return -1;
    }
    uint16_t inst = (regNum << 13) | ((opcode & 0x1F) << 8) | (imm & 0xFF);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

int genMov(FILE* outputFile, uint8_t opcode, const char* reg1, const char* reg2) {
    int reg1Num = checkRegister(reg1);
    int reg2Num = checkRegister(reg2);
    if (reg1Num == -1) {
        setErrorContext(lineCnt, reg1, "Invalid operand");
        return -1;
    }
    if (reg2Num == -1) {
        setErrorContext(lineCnt, reg2, "Invalid operand");
        return -1;
    }
    uint16_t inst = ((opcode & 0x1F) << 8) | ((reg1Num & 0x0F) << 4) | (reg2Num & 0x0F);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

int genArit(FILE* outputFile, uint8_t opcode, const char* reg1, const char* reg2, const char* reg3) {
    int reg1Num = checkRegister(reg1);
    int reg2Num = checkRegister(reg2);
    int reg3Num = reg3 ? checkRegister(reg3) : 0;
    if (reg1Num == -1 || reg1Num > 7) {
        setErrorContext(lineCnt, reg1, "Invalid operand");
        return -1;
    }
    if (reg2Num == -1 || reg2Num > 7) {
        setErrorContext(lineCnt, reg2, "Invalid operand");
        return -1;
    }
    if (reg3Num == -1 || reg3Num > 7) {
        setErrorContext(lineCnt, reg3, "Invalid operand");
        return -1;
    }
    uint16_t inst = (reg1Num << 13) | ((opcode & 0x1F) << 8) | ((reg2Num & 0x07) << 4) | (reg3Num & 0x07);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

int genJumpPushPop(FILE* outputFile, uint8_t opcode, const char* operand) {
    int regNum = checkRegister(operand);
    if (regNum == -1 || regNum > 7) {
        setErrorContext(lineCnt, operand, "Invalid operand");
        return -1;
    }
    uint16_t inst = (regNum << 13) | ((opcode & 0x1F) << 8);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

int genOthers(FILE* outputFile, uint8_t opcode) {
    uint16_t inst = ((opcode & 0x1F) << 8);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}