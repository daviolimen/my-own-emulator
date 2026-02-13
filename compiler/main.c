#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

char* getFile(const char* path) {
    FILE* file = fopen(path, "rb");

    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (length == -1) {
        perror("Error getting file size");
        fclose(file);
        return NULL;
    }

    char* buffer = (char*) malloc(length + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        free(buffer);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, length, file);
    fclose(file);

    if (bytesRead != (size_t) length) {
        perror("Error reading file");
        free(buffer);
        return NULL;
    }

    buffer[length] = '\0';
    return buffer;
}

int main(int argc, char** argv) {

    if (argc != 2) {
        puts("Usage: ./compiler [C file path]");
        return -1;
    }

    char* sourceCode = getFile(argv[1]);
    if (sourceCode == NULL) {
        puts("Error reading file");
        free(sourceCode);
        return -1;
    }

    Lexer lex;
    lexerInit(&lex, sourceCode);
    currentToken = lexerNext(&lex);

    free(sourceCode);
    return 0;
}