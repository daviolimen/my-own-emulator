// I started doing this as a single-file project and ended up

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "codegen.h"

#define MAX_LINE_LENGTH 256

// Struct to store the instructions
typedef struct {
    const char mnemonic[5];
    uint8_t opcode;
} instruction;

// Instructions ordered alphabetically, so that the assembler can match the mnemonics in O(log N) through binary search
instruction instructions[] = {
    {"ADD", 4},
    {"AND", 6},
    {"CMP", 11},
    {"HLT", 21},
    {"JEQ", 13},
    {"JGE", 16},
    {"JGT", 15},
    {"JLE", 18},
    {"JLT", 17},
    {"JMP", 12},
    {"JNE", 14},
    {"LDI", 23}, // this is not a real instruction, just used on the assembly to simplify things
    {"LLI", 0},
    {"LUI", 1},
    {"MOV", 2},
    {"NOP", 22},
    {"NOT", 3},
    {"OR", 7},
    {"POP", 20},
    {"PUSH", 19},
    {"SHL", 9},
    {"SHR", 10},
    {"SUB", 5},
    {"XOR", 8},
};

// Function to find the keyword in O(log N) using binary search
int findInstruction(const char* mnemonic) {
    int left = 0, right = sizeof(instructions) / sizeof(instructions[0]);
    while (right - left > 1) {
        int mid = (left + right) / 2; // no need for overflow trick as the array is small
        if (strcmp(instructions[mid].mnemonic, mnemonic) <= 0) left = mid;
        else right = mid;
    }
    if (strcmp(instructions[left].mnemonic, mnemonic) == 0) return left;
    return -1;
}

// Utility function to open a file using fopen
FILE* openInputFile(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }

    return file;
}

FILE* openOutputFile(const char* filePath) {
    FILE* file = fopen(filePath, "wb");
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }

    return file;
}

// Function for the first pass, used to parse and store the labels
int firstPass(FILE* inputFile) {
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL) {
        lineCnt++;

        char* comment = strchr(line, ';'); // find where the comment starts if there is one
        if (comment != NULL) *comment = '\0'; // turn the semicolon into \0 so that the comment is ignored

        char* token = strtok(line, "\t\n\r\f\v ,");

        if (token == NULL) continue;

        size_t len = strlen(token);
        if (token[len - 1] == ':') {
            token[len - 1] = '\0';
            addLabel(token, instructionCnt);
            token = strtok(NULL, "\t\n\r\f\v ,");
        }

        if (token == NULL) continue;

        if (findInstruction(token) == -1) {
            setErrorContext(lineCnt, token, "Invalid token");
            return -1;
        }

        if (strcmp(token, "LDI") == 0) instructionCnt += 2;
        else instructionCnt++;
    }

    return 0;
}

int secondPass(FILE* inputFile, FILE* outputFile) {
    lineCnt = 0;
    instructionCnt = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, inputFile) != NULL) {
        lineCnt++;

        char* comment = strchr(line, ';');
        if (comment != NULL) *comment = '\0';

        char* token = strtok(line, "\t\n\r\f\v ,");
        if (token == NULL) continue;

        size_t len = strlen(token);
        if (token[len - 1] == ':') token = strtok(NULL, "\t\n\r\f\v ,");

        if (token == NULL) continue;

        int idx = findInstruction(token);
        if (idx == -1) { // this should never happen because it should have been caught on the first pass
            setErrorContext(lineCnt, token, "Should not happen");
            return -1;
        }

        uint8_t opcode = instructions[idx].opcode;
        const char* op1 = strtok(NULL, "\t\n\r\f\v ,");
        const char* op2 = strtok(NULL, "\t\n\r\f\v ,");
        const char* op3 = strtok(NULL, "\t\n\r\f\v ,");
        printf("%d %s %s %s\n", opcode, op1, op2, op3);

        if (genCode(outputFile, opcode, op1, op2, op3) == -1) return -1;
    }

    return 0;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        puts("Usage: ./assembler [asm path] [bin path]");
        return -1;
    }

    FILE* inputFile = openInputFile(argv[1]);
    if (inputFile == NULL) {
        printf("Unable to open input file\n");
        return -1;
    }

    if (firstPass(inputFile) == -1) {
        printError();
        return -1;
    }

    FILE* outputFile = openOutputFile(argv[2]);
    if (outputFile == NULL) {
        printf("Unable to open/create output file\n");
        return -1;
    }

    rewind(inputFile);

    if (secondPass(inputFile, outputFile) == -1) {
        printError();
        return -1;
    }

    printf("Code was successfully assembled to %s\n", argv[2]);
    return 0;
}