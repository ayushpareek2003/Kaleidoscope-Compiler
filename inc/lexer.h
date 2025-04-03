#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>

// The lexer returns tokens [0-255] if it is an unknown character, otherwise one
// of these for known things.
enum Token {
    TokenEOF = -1,

    // commands
            TokenDef = -2, TokenExtern = -3,

    // primary
            TokenIdentifier = -4, TokenNumber = -5,
};

class Lexer {
    std::istream* inputStream;
    int lastChar = ' ';

protected:
    std::string identifier; // Filled in if TokenIdentifier
    double numberValue;     // Filled in if TokenNumber

public:
    Lexer() { }

    Lexer(std::istream* inputStream) :
            inputStream(inputStream) { }

    virtual int const getToken();
    std::string getIdentifier();
    double getNumberValue();
};

#endif