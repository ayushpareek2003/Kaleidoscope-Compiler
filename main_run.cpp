#include <iostream>
#include "inc/lexer.h"
#include "inc/parser.h"

int main() {
    Lexer lexer(&std::cin);
    Parser parser(&lexer);

    parser.binaryOpPrecedence['<'] = 10;
    parser.binaryOpPrecedence['+'] = 20;
    parser.binaryOpPrecedence['-'] = 20;
    parser.binaryOpPrecedence['*'] = 40;

    std::cerr << "ready> ";
    parser.getNextToken();

    parser.parse();

    return 0;
}