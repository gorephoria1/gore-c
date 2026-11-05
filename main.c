
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

// Pull in code generator reference symbols
void generateCode(ASTNode* node, FILE* out);

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [path_to_script.gore]\n", argv[0]);
        exit(64);
    }

    char* source = readFile(argv[1]);

    Lexer lexer;
    initLexer(&lexer, source);

    Parser parser;
    initParser(&parser, &lexer);

    // 1. Open up a bridge file to write our target C compilation strings
    FILE* outFile = fopen("output.c", "w");
    if (outFile == NULL) {
        fprintf(stderr, "Could not create intermediate source file output.c\n");
        free(source);
        exit(1);
    }

    printf("--- Compiling Script: %s -> gore_binary ---\n", argv[1]);

    // 2. Emit global target C library headers and entry runtime wrappers
    fprintf(outFile, "#include <stdio.h>\n\n");
    fprintf(outFile, "int main() {\n");

    // 3. Walk through statements recursively and emit C code instructions
    while (parser.current.type != TOKEN_EOF && !parser.hadError) {
        ASTNode* statementTree = parseStatement(&parser);
        if (statementTree != NULL && !parser.hadError) {
            generateCode(statementTree, outFile);
        }
    }

    // 4. Properly seal the runtime entry loop
    fprintf(outFile, "    return 0;\n");
    fprintf(outFile, "}\n");
    fclose(outFile);
    free(source);

    if (parser.hadError) {
        fprintf(stderr, "Compilation aborted due to previous syntax mistakes.\n");
        remove("output.c");
        exit(1);
    }

    // 5. AUTOMATION BOOTSTRAP: Invoke system native gcc to assemble the binary artifact
    printf("Invoking system native toolchain to assemble machine code...\n");
    int systemResult = system("gcc output.c -o gore_binary");

    if (systemResult == 0) {
        printf("Success! Created standalone binary: ./gore_binary\n");
        // Quietly drop the intermediate output source to keep your workspace pristine
        remove("output.c");
    } else {
        fprintf(stderr, "Backend Assembly Error: System compilation failure.\n");
        exit(1);
    }

    return 0;
}

