//
// Created by 冯泽灵 on 2017/1/2.
//
#include <string>

#ifndef PROLOG0_TOKENS_H
#define PROLOG0_TOKENS_H

#define TOKEN_LIST(T)  \
    T(LPAREN, "(")     \
    T(RPAREN, ")")     \
    T(COMMA, ",")      \
    T(PERIOD, ".")     \
    T(CUT, "!")        \
    T(COLONDASH, ":-") \
    T(ATOM, 0)         \
    T(VARIABLE, 0)     \


class token {
public:
    enum token_type {
#define T(t, v) t,
        TOKEN_LIST(T)
        NUM_TOKENS
#undef T
    };

private:
    token_type tok_;
    const char* value_;
};

const char *get_name(token::token_type);

#endif //PROLOG0_TOKENS_H
