#include <iostream>
#include <sstream>
#include "scanner.h"
#include "token.h"

#include "ast.h"
#include "wam.h"
#include "parser.h"
#include "rtti.h"
#include "inst.h"
#include "llvm/Support/Casting.h"

int main() {
    using namespace prolog0;
    std::istringstream is("person(zeling, male, shanghai) ?-  person(X, Y, shanghai).\nperson(X):-hello. ?- person(Y).");
    scanner sc(is);
    token tok;

    term *t = new variable("hello");
    std::unique_ptr<term> holder(t);

    assert(!dyn_cast<constant>(t));
    if (auto p = dyn_cast<variable>(t)) {
        std::cout << p->name << std::endl;
    }

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



    std::array<inst *, 3> insts = { new put_structure(functor("f", 2), 0), new put_variable(1, 2), new put_value(3, 4)};
    for (auto i : insts) {
       std::cout << *i << std::endl;
    }

    auto ps = llvm::dyn_cast<put_structure>(insts[0]);
    std::cout << *ps << std::endl;

    wam m;
    std::fill(m.real_heap_base(), m.real_heap_base() + 10, 'A');
    m.real_heap_base()[10] = '\0';
    std::cout << m.real_heap_base();
    return 0;
}
