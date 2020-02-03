#pragma once

#include "ast.h"
#include <optional>
#include <string>

#define TOKEN_LIST(T)                                                          \
    T(LPAREN, "(")                                                             \
    T(RPAREN, ")")                                                             \
    T(COMMA, ",")                                                              \
    T(PERIOD, ".")                                                             \
    T(CUT, "!")                                                                \
    T(COLONDASH, ":-")                                                         \
    T(QMDASH, "?-")                                                            \
                                                                               \
    T(FUNCTOR, 0)                                                              \
    T(VARIABLE, 0)                                                             \
                                                                               \
    T(EOS, "EOS")

class token {

  public:
    enum type {
#define T(t, v) t,
        TOKEN_LIST(T) NUM_TOKENS
#undef T
    };

  private:
    type _type;

    /*
    struct pos {
        unsigned long row;
        unsigned long col;
        std::string filename;

        pos(unsigned long row, unsigned long col, std::string filename)
                : row(row), col(col), filename(std::move(filename)) {}

        pos(const pos &) = delete;
        pos(pos &&) = default;
        pos &operator=(const pos &) = delete;
        pos &operator=(pos &&) = default;
    } _pos;
     */

    std::optional<std::string> _literal = {};

  public:
    /*
    token(type type, unsigned long row, unsigned long col, std::string filename,
    std::string literal) : _type(type), _pos(row, col, std::move(filename)),
    _literal(std::move(literal)) {}
        */
    token() : _type(NUM_TOKENS) {}
    token(type type) : _type(type) {}
    token(type type, std::string literal)
        : _type(type), _literal(std::move(literal)) {}
    token(const token &) = delete;
    token(token &&) = default;
    token &operator=(token &&) = default;
    token &operator=(const token &) = delete;

    std::string literal();
    std::string name() const;
    type type() const { return _type; }

    bool operator==(const token &rhs) {
        if (_type != rhs._type) {
            return false;
        }

        if (_type == VARIABLE || _type == FUNCTOR) {
            return *_literal == *rhs._literal;
        }

        return true;
    }

    bool operator!=(const token &rhs) { return !(*this == rhs); }
};
