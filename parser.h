#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef enum {
    NODE_LITERAL,    
    NODE_VARIABLE,   
    NODE_BINARY,     
    NODE_ASSIGN,     
    NODE_VAR_DECL,   
    NODE_PRINT       
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        double numberValue;
        struct {
            const char* name;
            int length;
            int isMutable; 
        } variable;
        struct {
            TokenType op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {
            const char* name;
            int nameLength;
            struct ASTNode* value;
        } assignment;
    } as;
} ASTNode;

typedef struct {
    Lexer* lexer;
    Token current;
    Token previous;
    int hadError;
} Parser;

void initParser(Parser* parser, Lexer* lexer);
ASTNode* parseStatement(Parser* parser);

#endif // PARSER_H

