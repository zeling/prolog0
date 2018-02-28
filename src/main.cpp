#include <iostream>
#include <sstream>
#include "scanner.h"
#include "token.h"

#include "ast.h"
#include "wam.h"
#include "parser.h"
#include "rtti.h"

//variable v("hello");

int main() {
    std::istringstream is("person(zeling, male, shanghai) ?-  person(X, Y, shanghai).\nperson(X):-hello. ?- person(Y).");
    scanner sc(is);
    token tok;

    ast::term *t = new ast::variable("hello");

    assert(!dyn_cast<ast::constant>(t));
    if (auto p = dyn_cast<ast::variable>(t)) {
        std::cout << p->name;
    }

//    term t = make_structure("f", { make_structure("g", { make_variable("X"), make_atom("u") }), make_variable("Y"), make_atom("v") });
//    std::cout << t << std::endl;

    parser p(sc);

    do {
        tok = sc.next();
        switch (tok.type()) {
            case token::FUNCTOR:
            case token::VARIABLE:
                std::cout << tok.literal() << ' ';
                break;
            default:
                std::cout << tok.name() << ' ';
            case token::NUM_TOKENS:
                break;
        }
    } while (tok.type() != token::EOS);

    wam m;
    std::fill(m.real_heap_base(), m.real_heap_base() + 10, 'A');
    m.real_heap_base()[10] = '\0';
    std::cout << m.real_heap_base();
    return 0;
}
