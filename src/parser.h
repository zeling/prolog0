#pragma once

#include <exception>
#include "scanner.h"
#include "ast.h"

class parser_error : public std::runtime_error {
public:
    parser_error(const char *str): std::runtime_error(str) {}
    parser_error(const std::string &str): std::runtime_error(str) {}
};

class parser {
    scanner &_scanner;

    template<typename P, typename Pred, typename Result=std::result_of_t<P()>>
    std::vector<Result> parse_list(P p, token delimiter, Pred end);
public:
    parser(scanner &scanner): _scanner(scanner) {}
    parser(const parser &) = delete;
    parser(parser&&) = default;

    term parse_term();
    term parse_query();
};

