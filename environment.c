#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "environment.h"

static int matchName(const char* s1, int len1, const char* s2, int len2) {
    if (len1 != len2) return 0;
    return strncmp(s1, s2, len1) == 0;
}

void initEnvironment(Environment* env) {
    env->head = NULL;
}

void envSet(Environment* env, const char* name, int length, double value, int isMutable) {
    Variable* current = env->head;
    
    while (current != NULL) {
        if (matchName(current->name, current->length, name, length)) {
            if (!current->isMutable) {
                printf("Runtime Error: Cannot reassign immutable variable '%.*s'.\n", length, name);
                return;
            }
            current->value = value;
            return;
        }
        current = current->next;
    }

    Variable* var = (Variable*)malloc(sizeof(Variable));
    var->name = name;
    var->length = length;
    var->value = value;
    var->isMutable = isMutable;
    var->next = env->head;
    env->head = var;
}

int envGet(Environment* env, const char* name, int length, double* outValue) {
    Variable* current = env->head;
    while (current != NULL) {
        if (matchName(current->name, current->length, name, length)) {
            *outValue = current->value;
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void freeEnvironment(Environment* env) {
    Variable* current = env->head;
    while (current != NULL) {
        Variable* next = current->next;
        free(current);
        current = next;
    }
    env->head = NULL;
}

