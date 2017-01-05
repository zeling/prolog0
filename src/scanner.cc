//
// Created by 冯泽灵 on 2017/1/2.
//

#include "scanner.h"

static void collect_literal(std::istream &is, char first, char *buf);


/* return tokens, NUM_TOKENS is returned if not recognized */
token::token_type scanner::next() {
    using tok = token::token_type;
    char ch;
    is_.get(ch);
    switch (ch) {
        case '(':
            return tok::LPAREN;
        case ')':
            return tok::RPAREN;
        case ',':
            return tok::COMMA;
        case '.':
            return tok::PERIOD;
        case '!':
            return tok::CUT;
        case ':':
            is_.get(ch);
            if (ch == '-') return tok::COLONDASH;
            else return tok::NUM_TOKENS;
        default:
            if (std::isupper(ch)) {
                collect_literal(is_, ch, literal_buffer_);
                return tok::VARIABLE;
            } else if (std::islower(ch)) {
                collect_literal(is_, ch, literal_buffer_);
                return tok::ATOM;
            } else {
                return tok::NUM_TOKENS;
            }
    }
}

std::string scanner::current_literal() {
    return { literal_buffer_ };
}

void scanner::skip_whitespace() {
    char ch;
    do {
        is_.get(ch);
    } while (std::isspace(ch));
    is_.unget();
}

void collect_literal(std::istream &is, char first, char *buf) {
    int lit_idx = 0;
    do {
        buf[lit_idx++] = first;
        is.get(first);
    } while (isalnum(first));
    is.unget();
    buf[lit_idx] = '\0';
}
