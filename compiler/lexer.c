//
// Created by davi on 2/9/26.
//

#include "lexer.h"

#include <ctype.h>
#include <string.h>

static const struct {
    const char* name;
    TokenType type;
} keywords[] = {
    {"if", TOK_IF},
    {"int", TOK_INT},
    {"while", TOK_WHILE},
    {"return", TOK_RETURN}
};

static char peek(Lexer* l) {
    return *l->cur;
}

static char advance(Lexer* l) {
    char c = *l->cur++;
    if (c == '\n') l->line++;
    return c;
}

static int match(Lexer* l, char expected) {
    if (*l->cur != expected) return 0;
    l->cur++;
    return 1;
}

static Token makeToken(Lexer* l, TokenType type, const char* start, int len) {
    Token t;
    t.type = type;
    t.start = start;
    t.length = len;
    t.line = l->line;
    t.value = 0;
    return t;
}

static Token getKeywordToken(Lexer* l, const char* start) {
    int len = (int) (l->cur - start);
    for (int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
        if (strncmp(start, keywords[i].name, len) == 0) {
            return makeToken(l, keywords[i].type, start, len);
        }
    }
    return makeToken(l, TOK_IDENT, start, len);
}

static Token number(Lexer* l) {
    const char* start = l->cur - 1;
    int value = *start - '0';

    while (isdigit(peek(l))) {
        value = value * 10 + (advance(l) - '0');
    }

    Token t = makeToken(l, TOK_NUMBER, start, (int) (l->cur - start));
    t.value = value;
    return t;
}

static Token identifier(Lexer* l) {
    const char* start = l->cur - 1;

    while (isalnum(peek(l)) || peek(l) == '_') advance(l);

    return getKeywordToken(l, start);
}

void lexerInit(Lexer* l, const char* source) {
    l->source = source;
    l->cur = source;
    l->line = 1;
}

Token lexerNext(Lexer* l) {
    for (;;) {
        char c = advance(l);

        switch (c) {
            case '\0':
                return makeToken(l, TOK_EOF, l->cur, 0);

            case ' ':
            case '\t':
            case '\r':
            case '\n':
                continue;

            case '+': return makeToken(l, TOK_PLUS, l->cur - 1, 1);
            case '-': return makeToken(l, TOK_MINUS, l->cur - 1, 1);
            case '*': return makeToken(l, TOK_STAR, l->cur - 1, 1);
            case '/': return makeToken(l, TOK_SLASH, l->cur - 1, 1);

            case '=':
                if (match(l, '=')) return makeToken(l, TOK_EQ, l->cur - 2, 2);
                return makeToken(l, TOK_ASSIGN, l->cur - 1, 1);

            case '!':
                if (match(l, '=')) return makeToken(l, TOK_NEQ, l->cur - 2, 2);
                break;

            case '<':
                if (match(l, '=')) return makeToken(l, TOK_LE, l->cur - 2, 2);
                return makeToken(l, TOK_LT, l->cur - 1, 1);

            case '>':
                if (match(l, '=')) return makeToken(l, TOK_GE, l->cur - 2, 2);
                return makeToken(l, TOK_GT, l->cur - 1, 1);

            case '(':
                return makeToken(l, TOK_LPAREN, l->cur - 1, 1);
            case ')':
                return makeToken(l, TOK_RPAREN, l->cur - 1, 1);
            case '{':
                return makeToken(l, TOK_LBRACE, l->cur - 1, 1);
            case '}':
                return makeToken(l, TOK_RBRACE, l->cur - 1, 1);
            case ';':
                return makeToken(l, TOK_SEMI, l->cur - 1, 1);

            default:
                if (isdigit(c)) return number(l);
                if (isalpha(c)) return identifier(l);
                break;
        }
    }
}