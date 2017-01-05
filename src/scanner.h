//
// Created by 冯泽灵 on 2017/1/2.
//

#ifndef PROLOG0_SCANNER_H
#define PROLOG0_SCANNER_H

#include "token.h"
#include <string>
#include <iostream>

class scanner {
public:
    explicit scanner(std::istream &in): is_(in) {}
    scanner(const scanner &scanner) = delete;
    scanner(scanner &&) = delete;
    token::token_type next();
    std::string current_literal();
    void skip_whitespace();
private:
    std::istream &is_;
    char literal_buffer_[256];
};


#endif //PROLOG0_SCANNER_H
