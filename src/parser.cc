#include "parser.h"
#include "ast.h"

namespace prolog0 {

std::unique_ptr<structure> parser::parse_structure() {
    token first = _scanner.next();
    if (first.type() != token::FUNCTOR) {
        throw parser_error("Expected FUNCTOR");
    }
    std::vector<std::unique_ptr<term>> args;
    token next = _scanner.next();
    if (next.type() == token::LPAREN) {
        args = parse_list([this] { return parse_term(); }, token(token::COMMA), token(token::RPAREN));
    } else {
        _scanner.push_back(std::move(next));
    }
    return std::make_unique<structure>(first.literal(), args.size(), std::move(args));
}

std::unique_ptr<term> parser::parse_term() {
    token first = _scanner.next();
    switch (first.type()) {
        case token::FUNCTOR: {
            _scanner.push_back(std::move(first));
            return parse_structure();
        }
        case token::VARIABLE:
            return std::make_unique<variable>(std::move(first.literal()));
        default:
            throw parser_error("expected FUNCTOR or VARIABLE");
    }
}

template<typename P, typename Result>
std::vector<Result> parser::parse_list(P p, token delimiter, token end) {
    token t;
    std::vector<Result> ret;
    while (true) {
        t = _scanner.next();
        if (t == end || t.type() == token::EOS) {
            return std::move(ret);
        }
        _scanner.push_back(std::move(t));
        ret.push_back(p());
        t = _scanner.next();
        if (t != delimiter) {
            if (t == end) {
                return std::move(ret);
            }
            throw parser_error("expected delimiter " + delimiter.name());
        }
    }
}

std::unique_ptr<query> parser::parse_query() {
    token t = _scanner.next();
    if (t.type() != token::QMDASH) {
        throw parser_error("expected QMDASH");
    }
    auto body = parse_list([this] { return this->parse_structure(); }, token(token::COMMA), token(token::PERIOD));
    return std::make_unique<query>(std::move(body));
}

std::unique_ptr<program> parser::parse_program() {
    auto head = parse_structure();
    token t = _scanner.next();
    if (t.type() == token::COLONDASH) {
        auto tail = parse_list([this] { return this->parse_structure(); }, token(token::COMMA), token(token::PERIOD));
        return std::make_unique<rule>(std::move(head), std::move(tail));
    } else if (t.type() == token::PERIOD) {
        return std::make_unique<fact>(std::move(head));
    }
    throw parser_error("no fact or rule detected");
}
}
