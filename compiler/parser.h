//
// Created by davi on 2/12/26.
//

#ifndef MY_OWN_EMULATOR_PARSER_H
#define MY_OWN_EMULATOR_PARSER_H
#include "token.h"
#include "lexer.h"

extern Token currentToken;

typedef enum {
    AST_ADD,
    AST_SUBTRACT,
    AST_NUMBER,
    AST_DECLARATION,
    AST_ASSIGNMENT,
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;
    union {
        int intValue;
        int declId;
        struct {
            char* ident;
            ASTNode* expr;
        } Assign;
        struct {
            char* name;
        } VarDecl;
    };
};

void parseStatements();

#endif //MY_OWN_EMULATOR_PARSER_H