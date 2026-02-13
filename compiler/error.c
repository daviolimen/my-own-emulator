//
// Created by davi on 2/12/26.
//

#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void reportError(int line, const char* errorDescription, const char* problem, int length) {
    printf("%s on line %d: %.*s\n", errorDescription, line, length, problem);
    exit(1);
}