#pragma once

#include <exception>
#include <memory>
#include "scanner.h"
#include "ast.h"

class parser_error : public std::runtime_error {
public:
    parser_error(const char *str): std::runtime_error(str) {}
    parser_error(const std::string &str): std::runtime_error(str) {}
};

class parser {
    scanner &_scanner;

    template<typename P, typename Result=std::result_of_t<P()>>
    std::vector<Result> parse_list(P p, token delimiter, token end);
public:
    parser(scanner &scanner): _scanner(scanner) {}
    parser(const parser &) = delete;
    parser(parser&&) = default;

    std::unique_ptr<ast::term> parse_term();
    std::unique_ptr<ast::query> parse_query();
    std::unique_ptr<ast::program> parse_program();
};

