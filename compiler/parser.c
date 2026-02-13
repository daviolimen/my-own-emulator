//
// Created by davi on 2/12/26.
//

#include "parser.h"

#include <stdlib.h>
#include <string.h>

int symbolNum = 0;
char* symTable[1024];

int findSymbol(const char* sym) {
    for (int i = 0; i < symbolNum; i++) {
        if (strcmp(symTable[i], sym) == 0) return i;
    }
    return -1;
}

int addSymbol(char* sym) {
    if (findSymbol(sym) != -1) return -1;
    symTable[symbolNum] = sym;
    symbolNum++;
    return 0;
}

Token currentToken;

void eat(Lexer* l, TokenType type) {
    if (currentToken.type == type) {
        currentToken = lexerNext(l);
    } else {
        reportError(l->line, "Unexpected token", currentToken.start, currentToken.length);
    }
}

ASTNode* parseVarDecl(Lexer* l) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_DECLARATION;

    eat(l, TOK_VAR);
    if (currentToken.type != TOK_IDENT) {
        reportError(l->line, "Expected identifier", currentToken.start, currentToken.length);
    }
    node->VarDecl.name = strndup(currentToken.start, currentToken.length);
    eat(l, TOK_IDENT);
    eat(l, TOK_SEMI);
    return node;
}

ASTNode* parseAssign(Lexer* l) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;

    if (currentToken.type != TOK_IDENT) {
        reportError(l->line, "Expected identifier", currentToken.start, currentToken.length);
    }
    node->Assign.ident = strndup(currentToken.start, currentToken.length);
    eat(l, TOK_IDENT);
    eat(l, TOK_EQUAL);
    node->Assign.expr = parseExpression(l);

}

void freeAST(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case AST_DECLARATION:
            free(node->VarDecl.name);
            break;
        case AST_ASSIGNMENT:
            freeAST(node->Assign.expr);
            free(node->Assign.ident);
            break;
    }
    free(node);
}

void parseStatements(Lexer* l) {
    while (1) {
        ASTNode* stmt = NULL;
        switch (currentToken.type) {
            case TOK_VAR:
                stmt = parseVarDecl(l);
                break;
            case TOK_IDENT:
                stmt = parseAssign(l);
                break;
            case TOK_EOF:
                return;
            default:
                reportError(currentToken.line, "Syntax error", currentToken.start, currentToken.length);
        }
        // generate code for the statement in question
        freeAST(stmt);
    }
}