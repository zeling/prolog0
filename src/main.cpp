#include <iostream>
#include <sstream>
#include "scanner.h"
#include "token.h"

int main() {
    std::istringstream is("person(zeling).\nperson(X):-hello. ?- person(Y).");
    scanner sc(is);
    token tok;
    do {
        tok = sc.next();
        switch (tok.type()) {
            case token::type::ATOM:
            case token::type::VARIABLE:
                std::cout << tok.literal() << ' ';
                break;
            default:
                std::cout << tok.name() << ' ';
            case token::type::NUM_TOKENS:
                break;
        }
    } while (tok.type() != token::type::EOS);
    return 0;
}
