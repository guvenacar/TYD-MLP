// c_compiler/c_parser.h

#ifndef C_PARSER_H
#define C_PARSER_H

#include "c_lexer.h"
#include "c_ast.h"

ASTNode* parse(const char* source_code);

// Parser'ın temel ayrıştırma fonksiyonları
ASTNode* ifade(); 
ASTNode* ikili_islem(int onceki_oncelik); 
ASTNode* birincil(); // Zaten mevcuttu, emin olmak için buraya ekliyorum

// Parser'ın ana işlevleri
ASTNode* parse(const char* source_code);

// Lexer'ın başlatılması (c_lexer.c'den)
void initLexer(const char* source);
Token* getNextToken();

#endif // C_PARSER_H