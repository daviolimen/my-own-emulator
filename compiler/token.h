//
// Created by davi on 2/9/26.
//

#ifndef MY_OWN_EMULATOR_TOKEN_H
#define MY_OWN_EMULATOR_TOKEN_H

typedef enum {
    TOK_EOF,

    TOK_INT,
    TOK_IF,
    TOK_WHILE,
    TOK_RETURN,

    TOK_IDENT,
    TOK_NUMBER,

    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_ASSIGN,
    TOK_EQ,
    TOK_NEQ,
    TOK_LT,
    TOK_LE,
    TOK_GT,
    TOK_GE,

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