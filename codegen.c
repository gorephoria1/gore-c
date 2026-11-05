#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

// Forward declaration of internal node emitter loop
static void generateExpression(ASTNode* node, FILE* out);

static void generateExpression(ASTNode* node, FILE* out) {
    if (node == NULL) return;

    switch (node->type) {
        case NODE_LITERAL:
            // Emit raw decimal numbers
            fprintf(out, "%g", node->as.numberValue);
            break;

        case NODE_VARIABLE:
            // Emit raw mapped variable identifier labels
            fprintf(out, "%.*s", node->as.variable.length, node->as.variable.name);
            break;

        case NODE_BINARY:
            fprintf(out, "(");
            generateExpression(node->as.binary.left, out);
            
            // Map token states directly to C operators
            switch (node->as.binary.op) {
                case TOKEN_PLUS:        fprintf(out, " + "); break;
                case TOKEN_MINUS:       fprintf(out, " - "); break;
                case TOKEN_STAR:        fprintf(out, " * "); break;
                case TOKEN_SLASH:       fprintf(out, " / "); break;
                case TOKEN_EQUAL_EQUAL: fprintf(out, " == "); break;
                case TOKEN_BANG_EQUAL:  fprintf(out, " != "); break;
                case TOKEN_LESS:        fprintf(out, " < "); break;
                case TOKEN_GREATER:     fprintf(out, " > "); break;
                default:                break;
            }
            
            generateExpression(node->as.binary.right, out);
            fprintf(out, ")");
            break;

        default:
            break;
    }
}

// Global router function to convert AST statements into executable target C
void generateCode(ASTNode* node, FILE* out) {
    if (node == NULL) return;

    switch (node->type) {
        case NODE_VAR_DECL:
            // Transpile variable scopes into fast double-precision types
            fprintf(out, "    double %.*s = ", node->as.assignment.nameLength, node->as.assignment.name);
            generateExpression(node->as.assignment.value, out);
            fprintf(out, ";\n");
            break;

        case NODE_PRINT:
            // Transpile print keywords natively into standard library printf communications
            fprintf(out, "    printf(\"%%g\\n\", (double)(");
            generateExpression(node->as.assignment.value, out);
            fprintf(out, "));\n");
            break;

        default:
            // Handle raw statement calculations if necessary
            fprintf(out, "    ");
            generateExpression(node, out);
            fprintf(out, ";\n");
            break;
    }
}

