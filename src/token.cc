//
// Created by 冯泽灵 on 2017/1/2.
//

#include "token.h"
#include <assert.h>

static const char *token_names[] = {
#define T(t, v) #t,
    TOKEN_LIST(T)
#undef T
};

std::string token::literal() {
    assert(bool(_literal));
    auto ret = std::move(*_literal);
    _literal = {};
    return std::move(ret);
}

std::string token::name() const { return token_names[_type]; }
