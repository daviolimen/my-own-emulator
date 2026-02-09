//
// Created by davi on 2/9/26.
//

#ifndef MY_OWN_EMULATOR_LEXER_H
#define MY_OWN_EMULATOR_LEXER_H

#include "token.h"

typedef struct {
    const char* source;
    const char* cur;
    int line;
} Lexer;

void lexerInit(Lexer* lex, const char* source);

Token lexerNext(Lexer* lex);

#endif //MY_OWN_EMULATOR_LEXER_H