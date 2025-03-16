#ifndef LEXER_H
#define LEXER_H

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>

enum Token {
    tok_eof = -1,
  
    
    tok_def = -2,
    tok_extern = -3,

    tok_identifier = -4,
    tok_number = -5,
  
 
    tok_if = -6,
    tok_then = -7,
    tok_else = -8,
    tok_for = -9,
    tok_in = -10
  };
  

extern std::string IdentifierStr;
extern double NumVal;

int gettok();

#endif
