#include <iostream>
#include <sstream>
#include "scanner.h"
#include "token.h"

#include "ast.h"
#include "parser.h"

//variable v("hello");

int main() {
    std::istringstream is("person(zeling, male, shanghai).\nperson(X):-hello. ?- person(Y).");
    scanner sc(is);
    token tok;

//    term t = make_structure("f", { make_structure("g", { make_variable("X"), make_atom("u") }), make_variable("Y"), make_atom("v") });
//    std::cout << t << std::endl;

    parser p(sc);

    try {
        std::cout << p.parse_term() << std::endl;
    } catch (const parser_error &e) {
        std::cout << e.what() << std::endl;
    }

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
