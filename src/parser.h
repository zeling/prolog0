#pragma once

#include "scanner.h"
#include <exception>

class parser_error : std::exception {
    std::string reason;
};

class parser {
    scanner &_scanner;
public:
    parser(scanner &scanner): _scanner(scanner) {}
    parser(const parser &) = delete;
    parser(parser&&) = default;


};

