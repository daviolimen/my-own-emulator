//
// Created by davi on 2/5/26.
//

#include "error.h"

typedef struct {
    uint16_t line;
    char lexicalElement[32];
    char errorType[32];
} error_context;

error_context error;

void setErrorContext(uint16_t line, const char* lexicalElement, const char* errorType) {
    error.line = line;
    strcpy(error.lexicalElement, lexicalElement);
    strcpy(error.errorType, errorType);
}

void printError() {
    printf("%s on line %d: %s\n", error.errorType, error.line, error.lexicalElement);
}