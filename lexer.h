#ifndef LEXER_H
#define LEXER_H

typedef enum {
    // Single-character tokens
    TOKEN_LPAREN, TOKEN_RPAREN,    
    TOKEN_LBRACE, TOKEN_RBRACE,    
    TOKEN_PLUS, TOKEN_MINUS,       
    TOKEN_STAR, TOKEN_SLASH,       
    TOKEN_ASSIGN, TOKEN_SEMICOLON, 
    TOKEN_LESS, TOKEN_GREATER,     

    // Two-character tokens
    TOKEN_EQUAL_EQUAL, TOKEN_BANG_EQUAL, 

    // Literals and Identifiers
    TOKEN_IDENTIFIER,              
    TOKEN_NUMBER,                  

    // Keywords
    TOKEN_LET,                     
    TOKEN_MUT,                     
    TOKEN_PRINT,                   
    
    // Control / System
    TOKEN_EOF,                     
    TOKEN_ERROR                    
} TokenType;

typedef struct {
    TokenType type;
    const char* start;             
    int length;                    
    int line;                      
} Token;

typedef struct {
    const char* start;
    const char* current;
    int line;
} Lexer;

void initLexer(Lexer* lexer, const char* source);
Token scanToken(Lexer* lexer);

#endif // LEXER_H

