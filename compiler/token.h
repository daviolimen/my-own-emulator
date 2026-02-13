//
// Created by davi on 2/9/26.
//

#ifndef MY_OWN_EMULATOR_TOKEN_H
#define MY_OWN_EMULATOR_TOKEN_H

typedef enum {
    TOK_EOF,

    TOK_VAR,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,

    TOK_IDENT,
    TOK_NUMBER,

    TOK_PLUS,
    TOK_MINUS,
    TOK_AND,
    TOK_OR,
    TOK_XOR,
    TOK_LSHIFT,
    TOK_RSHIFT,

    TOK_EQUAL,
    TOK_EQUAL_EQUAL,
    TOK_BANG,
    TOK_BANG_EQUAL,
    TOK_GREATER,
    TOK_GREATER_EQUAL,
    TOK_LESS,
    TOK_LESS_EQUAL,

    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMI,
} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
    int value;
} Token;

#endif //MY_OWN_EMULATOR_TOKEN_H