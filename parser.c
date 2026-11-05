#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

// Added PREC_COMPARISON to handle relational grammar rules correctly
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_COMPARISON,  // == != < >  👈 Add this tier
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_PRIMARY
} Precedence;

static ASTNode* parsePrecedence(Parser* parser, Precedence precedence);
static ASTNode* expression(Parser* parser);

static void advance(Parser* parser) {
    parser->previous = parser->current;

    for (;;) {
        parser->current = scanToken(parser->lexer);
        if (parser->current.type != TOKEN_ERROR) break;

        printf("[Line %d] Lexer Error: Unexpected token sequence.\n", parser->current.line);
        parser->hadError = 1;
    }
}

static void consume(Parser* parser, TokenType type, const char* message) {
    if (parser->current.type == type) {
        advance(parser);
        return;
    }

    printf("[Line %d] Syntax Error: %s\n", parser->current.line, message);
    parser->hadError = 1;
}

static ASTNode* allocateNode(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) {
        fprintf(stderr, "Fatal Error: Out of memory while generating AST node.\n");
        exit(1);
    }
    node->type = type;
    return node;
}

// Map the new comparison tokens to their respective precedence rank
static Precedence getPrecedence(TokenType type) {
    switch (type) {
        case TOKEN_PLUS:  case TOKEN_MINUS:   return PREC_TERM;
        case TOKEN_STAR:  case TOKEN_SLASH:   return PREC_FACTOR;
        case TOKEN_LESS:  case TOKEN_GREATER: 
        case TOKEN_EQUAL_EQUAL: case TOKEN_BANG_EQUAL: return PREC_COMPARISON; // 👈 Route them here
        default:                              return PREC_NONE;
    }
}

static ASTNode* primary(Parser* parser) {
    if (parser->current.type == TOKEN_NUMBER) {
        advance(parser);
        ASTNode* node = allocateNode(NODE_LITERAL);
        node->as.numberValue = strtod(parser->previous.start, NULL);
        return node;
    }

    if (parser->current.type == TOKEN_IDENTIFIER) {
        advance(parser);
        ASTNode* node = allocateNode(NODE_VARIABLE);
        node->as.variable.name = parser->previous.start;
        node->as.variable.length = parser->previous.length;
        node->as.variable.isMutable = 0;
        return node;
    }

    printf("[Line %d] Error: Expected expression value.\n", parser->current.line);
    parser->hadError = 1;
    return NULL;
}

static ASTNode* parsePrecedence(Parser* parser, Precedence precedence) {
    ASTNode* left = primary(parser);
    if (left == NULL) return NULL;

    while (precedence <= getPrecedence(parser->current.type)) {
        advance(parser);
        TokenType op = parser->previous.type;
        
        ASTNode* right = parsePrecedence(parser, (Precedence)(getPrecedence(op) + 1));
        
        ASTNode* binaryNode = allocateNode(NODE_BINARY);
        binaryNode->as.binary.op = op;
        binaryNode->as.binary.left = left;
        binaryNode->as.binary.right = right;
        
        left = binaryNode;
    }

    return left;
}

static ASTNode* expression(Parser* parser) {
    return parsePrecedence(parser, PREC_ASSIGNMENT);
}

static ASTNode* varDeclaration(Parser* parser, int isMutable) {
    consume(parser, TOKEN_IDENTIFIER, "Expected variable name after declaration keyword.");
    if (parser->hadError) return NULL;

    const char* varName = parser->previous.start;
    int varLength = parser->previous.length;

    consume(parser, TOKEN_ASSIGN, "Expected '=' assignment operator after variable name.");
    if (parser->hadError) return NULL;

    ASTNode* valueExpr = expression(parser);

    consume(parser, TOKEN_SEMICOLON, "Expected ';' ending delimiter after statement.");

    ASTNode* node = allocateNode(NODE_VAR_DECL);
    node->as.assignment.name = varName;
    node->as.assignment.nameLength = varLength;
    node->as.assignment.value = valueExpr;
    
    return node;
}

void initParser(Parser* parser, Lexer* lexer) {
    parser->lexer = lexer;
    parser->hadError = 0;
    advance(parser); 
}

ASTNode* parseStatement(Parser* parser) {
    if (parser->current.type == TOKEN_LET) {
        advance(parser);
        return varDeclaration(parser, 0); 
    } 
    if (parser->current.type == TOKEN_MUT) {
        advance(parser);
        return varDeclaration(parser, 1); 
    }
    if (parser->current.type == TOKEN_PRINT) {
        advance(parser);
        ASTNode* expr = expression(parser);
        consume(parser, TOKEN_SEMICOLON, "Expected ';' after print statement.");
        ASTNode* node = allocateNode(NODE_PRINT);
        node->as.assignment.value = expr;
        return node;
    }

    ASTNode* exprNode = expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after expression statement.");
    return exprNode;
}

