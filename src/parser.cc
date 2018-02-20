//
// Created by 冯泽灵 on 2017/1/2.
//

#include "parser.h"
#include "ast.h"

term parser::parse_term() {
    token first = _scanner.next();
    switch (first.type()) {
        case token::ATOM: {
            token next = _scanner.next();
            if (next.type() == token::LPAREN) {
                std::vector<term> args = parse_list([this] { return parse_term(); }, token(token::COMMA), [] (const token &t) {
                    return t.type() == token::RPAREN;
                });
                return make_structure(std::move(first.literal()), std::move(args));
            } else {
                _scanner.push_back(std::move(next));
                return make_atom(std::move(first.literal()));
            }
        }
        case token::VARIABLE:
            return make_variable(std::move(first.literal()));
        default:
            throw parser_error("expected ATOM or VARIABLE");
    }
}

template<typename P, typename Pred>
std::vector<term> parser::parse_list(P p, token delimiter, Pred end) {
    token t;
    std::vector<term> ret;
    while (true) {
        t = _scanner.next();
        if (end(t) || t.type() == token::EOS) {
            return std::move(ret);
        }
        _scanner.push_back(std::move(t));
        ret.push_back(p());
        t = _scanner.next();
        if (t != delimiter) {
            if (end(t)) {
                return std::move(ret);
            }
            throw parser_error("expected delimiter " + delimiter.name());
        }
    }
}

