//
// Created by 冯泽灵 on 2017/1/2.
//

#include "token.h"

static const char* token_values[] = {
#define T(t, v) v,
    TOKEN_LIST(T)
#undef T
};

static const char* token_names[] = {
#define T(t, v) #t,
    TOKEN_LIST(T)
#undef T
};

const char *get_name(token::token_type tok) {
    if (tok >= 0 && tok < token::token_type::NUM_TOKENS)
        return token_values[tok];
    else return nullptr;
}
