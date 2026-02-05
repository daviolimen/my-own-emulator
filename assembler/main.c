#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

uint16_t lineCnt = 0;
uint16_t instructionCnt = 0;

// Struct to store the instructions
typedef struct {
    const char mnemonic[5];
    uint8_t opcode;
} instruction;

// Keywords ordered alphabetically, so that the assembler can match the mnemonics in O(log N) through binary search
instruction keywords[] = {
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
    int left = 0, right = sizeof(keywords) / sizeof(keywords[0]);
    while (right - left > 1) {
        int mid = (left + right) / 2; // no need for overflow trick as the array is small
        if (strcmp(keywords[mid].mnemonic, mnemonic) <= 0) left = mid;
        else right = mid;
    }
    if (strcmp(keywords[left].mnemonic, mnemonic) == 0) return left;
    return -1;
}

// Struct to store labels
typedef struct {
    char label[32];
    uint16_t address;
} label;

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

// Utility function to open a file using fopen
FILE* openFile(const char* filePath) {
    FILE* file = fopen(filePath, "r");
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
            continue;
        }
        if (findInstruction(token) == -1) return -1;
        if (strcmp(token, "LDI") == 0) instructionCnt += 2;
        else instructionCnt++;
    }
    return 0;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        puts("Usage: ./assembler [asm path] [bin path]");
        return -1;
    }

    FILE* inputFile = openFile(argv[1]);
    if (inputFile == NULL) {
        printf("Unable to open input file\n");
        return -1;
    }

    if (firstPass(inputFile) == -1) {
        printf("Invalid keyword used in line %d\n", lineCnt);
        return -1;
    }
}