#pragma once

#include <iostream>
#include "token.h"

class scanner {
    std::istream &_input;
    char _literal_buffer[256];

    typedef enum token::type token_type;

    token_type do_scan();
    void do_collect_literal();

public:
    explicit scanner(std::istream &input): _input(input) {}
    token next();
};


