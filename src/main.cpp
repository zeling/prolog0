#include <iostream>
#include <sstream>
#include "scanner.h"
#include "token.h"

int main() {
    std::istringstream is("person(zeling).\nperson(X).");
    scanner sc(is);
    token::token_type tok;
    do {
        tok = sc.next();
        switch (tok) {
            case token::token_type::ATOM:
            case token::token_type::VARIABLE:
                std::cout << sc.current_literal() << ' ';
                break;
            default:
                std::cout << std::string{get_name(tok)} << ' ';
            case token::token_type::NUM_TOKENS:
                break;
        }
    } while (tok != token::token_type::EOS);
    return 0;
}