//
// Created by 冯泽灵 on 2017/1/2.
//

#include <algorithm>
#include "scanner.h"

/* return tokens, NUM_TOKENS is returned if not recognized */
scanner::token_type scanner::do_scan() {
    _input >> std::ws;
    char ch;
    _input.get(ch);
    switch (ch) {
        case '(':
            return token_type::LPAREN;
        case ')':
            return token_type::RPAREN;
        case ',':
            return token_type::COMMA;
        case '.':
            return token_type::PERIOD;
        case '!':
            return token_type::CUT;
        case ':':
            _input.get(ch);
            if (ch == '-') return token_type::COLONDASH;
            else return token_type::NUM_TOKENS;
        case '?':
            _input.get(ch);
            if (ch == '-') return token_type::QMDASH;
            else return token_type::NUM_TOKENS;
        case EOF:
        case '\0':
            return token_type::EOS;
        default:
            if (std::isupper(ch)) {
                _input.unget();
                do_collect_literal();
                return token_type::VARIABLE;
            } else if (std::islower(ch)) {
                _input.unget();
                do_collect_literal();
                return token_type::FUNCTOR;
            } else {
                return token_type::NUM_TOKENS;
            }
    }
}

void scanner::do_collect_literal() {
    char *d_out = _literal_buffer;
    auto it = std::istream_iterator<char>(_input);
    auto eof = std::istream_iterator<char>();
    while (it != eof && std::isalpha(*it)) {
        *d_out++ = *it++;
    }
    _input.unget();
    *d_out = '\0';
}

token scanner::next() {
    if (_look_ahead) {
        auto tok = std::move(*_look_ahead);
        _look_ahead = {};
        return std::move(tok);
    }
    token_type type = do_scan();
    if (type == token_type::FUNCTOR || type == token_type::VARIABLE) {
        return token(type, std::string(_literal_buffer));
    }
    return token(type);
}

void scanner::push_back(token t) {
    _look_ahead.emplace(std::move(t));
}
