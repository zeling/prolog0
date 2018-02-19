//
// Created by 冯泽灵 on 2017/1/2.
//

#include "scanner.h"

#ifndef PROLOG0_PARSER_H
#define PROLOG0_PARSER_H

class parser_error {
    std::string reason;
};

class parser {
public:
    parser(scanner &sc): sc_(sc) {}
    void parse_clause(parser_error *err);
    void parse_structure(parser_error *err);
    void parse_query(parser_error *err);

private:
    scanner &sc_;
};





#endif //PROLOG0_PARSER_H
