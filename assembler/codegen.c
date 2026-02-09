//
// Created by davi on 2/5/26.
//

#include "codegen.h"

#include <ctype.h>
#include <stdlib.h>

// Struct to store labels
typedef struct {
    char label[32];
    uint16_t address;
} label;

// Array for the labels
label labels[128];
int labelCnt = 0;

static const char* registersStrings[] = {
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "SP",
    "[R0]", "[R1]", "[R2]", "[R3]", "[R4]", "[R5]", "[R6]", "[SP]"
};

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

// Buffer to store the data defined in the program
uint8_t dataSection[0x2000];

// Function to process assembler directives
int processDirective(const char* directive, const char* line) {
    char* token = strtok(NULL, "\t\n\r\f\v ,");
    if (token == NULL) {
        setErrorContext(lineCnt, "", "Missing data label");
        return -1;
    }
    if (isdigit(token[0])) {
        setErrorContext(lineCnt, token, "Label cannot start with number");
        return -1;
    }
    char* endptr;
    addLabel(token, dataCnt);
    if (strcmp(directive, ".db") == 0) {
        while ((token = strtok(NULL, "\t\n\r\f\v ,"))) {
            uint8_t num = (uint8_t) strtol(token, &endptr, 0);
            if (*endptr != '\0') {
                setErrorContext(lineCnt, token, "Invalid data");
                return -1;
            }
            dataSection[dataCnt++] = num;
        }
    } else if (strcmp(directive, ".dw") == 0) {
        while ((token = strtok(NULL, "\t\n\r\f\v ,"))) {
            uint16_t num = (uint16_t) strtol(token, &endptr, 0);
            if (*endptr != '\0') {
                setErrorContext(lineCnt, token, "Invalid data");
                return -1;
            }
            dataSection[dataCnt++] = num;
            dataSection[dataCnt++] = num >> 8;
        }
    } else if (strcmp(directive, ".ascii") == 0) {
        token = strtok(NULL, "\t\n\r\f\v ," );
        if (token == NULL) return 0;
        if (token[0] != '"') {
            setErrorContext(lineCnt, token, "Missing opening quote");
            return -1;
        }

        // as we can only have one string literal in the line, I get the first and last quote I find, this way the
        // assembler can handle quotes inside the string quotes
        size_t idx1 = 0;
        for (size_t i = 0; i < MAX_LINE_LENGTH; i++) {
            if (line[i] == '"') {
                idx1 = i;
                break;
            }
        }
        size_t idx2 = 0;
        for (size_t i = MAX_LINE_LENGTH - 1; i > idx1; i--) {
            if (line[i] == '"') {
                idx2 = i;
                break;
            }
        }
        if (idx2 == 0) {
            setErrorContext(lineCnt, "", "Missing closing quote");
            return -1;
        }

        for (size_t i = idx1 + 1; i < idx2; i++) dataSection[dataCnt++] = line[i];
    } else if (strcmp(directive, ".alloc") == 0) {
        token = strtok(NULL, "\t\n\r\f\v ,");
        uint16_t num = (uint16_t) strtol(token, &endptr, 0);
        if (*endptr != '\0') {
            setErrorContext(lineCnt, token, "Invalid data");
            return -1;
        }
        dataCnt += num;
    } else {
        setErrorContext(lineCnt, directive, "Invalid directive");
        return -1;
    }
    return 0;
}

// Function to check if a register operand is valid and return its value
static int checkRegister(const char* reg) {
    for (uint8_t i = 0; i < 16; i++) {
        if (strcmp(reg, registersStrings[i]) == 0) return i;
    }
    return -1;
}

static int genLoad(FILE* outputFile, uint8_t opcode, const char* reg, uint8_t imm) {
    int regNum = checkRegister(reg);
    if (regNum == -1 || regNum > 7) {
        setErrorContext(lineCnt, reg, "Invalid operand");
        return -1;
    }
    uint16_t inst = (regNum << 13) | ((opcode & 0x1F) << 8) | (imm & 0xFF);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

static int genMov(FILE* outputFile, uint8_t opcode, const char* reg1, const char* reg2) {
    if (reg1 == NULL) {
        setErrorContext(lineCnt, "", "Missing operand 1");
        return -1;
    }
    if (reg2 == NULL) {
        setErrorContext(lineCnt, "", "Missing operand 2");
        return -1;
    }
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

static int genShift(FILE* outputFile, uint8_t opcode, int reg1Num, int reg2Num, const char* imm) {
    char* endptr;
    uint8_t immNum = strtol(imm, &endptr, 0);
    if (*endptr != '\0') {
        setErrorContext(lineCnt, imm, "Invalid operand");
        return -1;
    }

    uint16_t inst = (reg1Num << 13) | ((opcode & 0x1F) << 8) | ((reg2Num & 0x07) << 4) | (immNum & 0x0F);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

static int genArit(FILE* outputFile, uint8_t opcode, const char* reg1, const char* reg2, const char* reg3) {
    if (reg1 == NULL) {
        setErrorContext(lineCnt, "", "Missing operand 1");
        return -1;
    }
    if (reg2 == NULL) {
        setErrorContext(lineCnt, "", "Missing operand 2");
        return -1;
    }
    if ((opcode >= 4) && (opcode <= 10) && (reg3 == NULL)) {
        setErrorContext(lineCnt, "", "Missing operand 3");
        return -1;
    }

    int reg1Num = checkRegister(reg1);
    int reg2Num = checkRegister(reg2);

    if (opcode >= 9 && opcode <= 10) { // I had forgotten of the shifts before, so I am doing it now...
        return genShift(outputFile, opcode, reg1Num, reg2Num, reg3);
    }

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

static int genJumpPushPop(FILE* outputFile, uint8_t opcode, const char* operand) {
    if (operand == NULL) {
        setErrorContext(lineCnt, "", "Missing operand");
        return -1;
    }

    int regNum = checkRegister(operand);
    if (regNum == -1 || regNum > 7) {
        setErrorContext(lineCnt, operand, "Invalid operand");
        return -1;
    }
    uint16_t inst = (regNum << 13) | ((opcode & 0x1F) << 8);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

static int genOthers(FILE* outputFile, uint8_t opcode) {
    uint16_t inst = ((opcode & 0x1F) << 8);
    fwrite(&inst, sizeof(uint16_t), 1, outputFile);
    return 0;
}

// Function to generate the instructions, there are faster ways than using lots of if-statements, like using an array
// of function addresses, as in the emulator, but there is no need to complicate things here
int genCode(FILE* outputFile, uint8_t opcode, const char* op1, const char* op2, const char* op3) {
    char* endptr;
    if (opcode < 2) {
        if (op2 == NULL) {
            setErrorContext(lineCnt, "", "Missing operand 2");
            return -1;
        }
        uint8_t imm = strtol(op2, &endptr, 0);
        if (*endptr == '\0') return genLoad(outputFile, opcode, op1, imm);
        setErrorContext(lineCnt, op2, "Invalid operand");
        return -1;
    }
    if (opcode == 2) return genMov(outputFile, opcode, op1, op2);
    if (opcode < 12) return genArit(outputFile, opcode, op1, op2, op3);
    if (opcode < 21) return genJumpPushPop(outputFile, opcode, op1);
    if (opcode < 23) return genOthers(outputFile, opcode);

    if (op2 == NULL) {
        setErrorContext(lineCnt, "", "Missing operand 2");
        return -1;
    }

    uint16_t imm;
    if ((op2[0] >= '0') && (op2[0] <= '9')) {
        imm = strtol(op2, &endptr, 0);
        if (*endptr != '\0') {
            setErrorContext(lineCnt, op2, "Invalid operand");
            return -1;
        }
    } else {
        imm = findLabel(op2);
        if (imm == (uint16_t) -1) {
            setErrorContext(lineCnt, op2, "Invalid label");
            return -1;
        }
    }
    return genLoad(outputFile, 0, op1, imm & 0xFF) | genLoad(outputFile, 1, op1, imm >> 8);
}
