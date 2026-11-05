#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "environment.h"

double evaluate(ASTNode* node, Environment* env) {
    if (node == NULL) return 0.0;

    switch (node->type) {
        case NODE_LITERAL:
            return node->as.numberValue;

        case NODE_VARIABLE: {
            double value;
            if (envGet(env, node->as.variable.name, node->as.variable.length, &value)) {
                return value;
            }
            printf("Runtime Error: Undefined variable '%.*s'.\n", 
                   node->as.variable.length, node->as.variable.name);
            exit(1);
        }

        case NODE_BINARY: {
            double left = evaluate(node->as.binary.left, env);
            double right = evaluate(node->as.binary.right, env);
            
            switch (node->as.binary.op) {
                // Mathematical operations
                case TOKEN_PLUS:  return left + right;
                case TOKEN_MINUS: return left - right;
                case TOKEN_STAR:  return left * right;
                case TOKEN_SLASH: 
                    if (right == 0.0) {
                        printf("Runtime Error: Division by zero.\n");
                        exit(1);
                    }
                    return left / right;
                
                // New Logical Comparison Operations (1.0 = True, 0.0 = False)
                case TOKEN_EQUAL_EQUAL: return (left == right) ? 1.0 : 0.0;
                case TOKEN_BANG_EQUAL:  return (left != right) ? 1.0 : 0.0;
                case TOKEN_LESS:        return (left < right)  ? 1.0 : 0.0;
                case TOKEN_GREATER:     return (left > right)  ? 1.0 : 0.0;

                default: return 0.0;
            }
        }

        case NODE_VAR_DECL: {
            double val = evaluate(node->as.assignment.value, env);
            envSet(env, node->as.assignment.name, node->as.assignment.nameLength, val, 1);
            return val;
        }

        case NODE_PRINT: {
            double value = evaluate(node->as.assignment.value, env);
            printf("%g\n", value);
            return value;
        }

        default:
            return 0.0;
    }
}

