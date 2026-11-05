#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

typedef struct Variable {
    const char* name;
    int length;
    double value;
    int isMutable;
    struct Variable* next;
} Variable;

typedef struct {
    Variable* head;
} Environment;

void initEnvironment(Environment* env);
void envSet(Environment* env, const char* name, int length, double value, int isMutable);
int envGet(Environment* env, const char* name, int length, double* outValue);
void freeEnvironment(Environment* env);

#endif // ENVIRONMENT_H

