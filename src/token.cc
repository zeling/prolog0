//
// Created by 冯泽灵 on 2017/1/2.
//

#include "token.h"

enum token_type {
#define T(t, v) t
    TOKEN_LIST(T),
#undef T
    NUM_TOKENS
};

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

static const char *get_name(token::token_type tok) {
    if (tok >= 0 && tok < NUM_TOKENS)
        return token_values[tok];
}

