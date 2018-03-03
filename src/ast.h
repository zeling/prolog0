#pragma once

#include <vector>
#include <string>
#include <memory>
#include <initializer_list>
#include <assert.h>

#include "util.h"



namespace ast {

    class term {
    public:
        enum kind {
            variable,
            structure,
            constant
        };
    private:
        const kind _kind;
    public:
        term() = default;
        term(kind k): _kind(k) {}
        kind kind() const {
            return _kind;
        }
    };

    struct variable : public term {
        std::string name;

        variable(std::string name) : term(term::variable), name(std::move(name)) {}
        static bool classof(const term *t) {
            return t->kind() == term::variable;
        }
    };

    struct functor {
        std::string name;
        size_t arity;

        functor(std::string name, size_t arity): name(std::move(name)), arity(arity) {}
    };


    struct structure : public term {
        struct functor functor;
        std::vector<std::unique_ptr<term>> args;

        structure(std::string name, size_t arity, std::vector<std::unique_ptr<term>> args):
                term(term::structure), args(std::move(args)), functor(std::move(name), arity) {
        }

        static bool classof(term *t) {
            return t->kind() == term::structure;
        }
    };

    struct constant : public term {
        std::string name;

        constant(std::string name): term(term::constant) {}

        static bool classof(term *t) {
            return t->kind() == term::constant;
        }
    };

    class program {
    public:
        enum kind {
            fact,
            rule,
        };
    private:
        const kind _kind;
    public:
        program(kind k): _kind(k) {}
        kind kind() {
            return _kind;
        }
    };

    struct fact : public program {
        std::unique_ptr<term> _term;
    public:
        fact(std::unique_ptr<term> t): program(program::fact), _term(std::move(t)) {}
        static bool classof(program *p) {
            return p->kind() == program::fact;
        }
    };

    struct rule : public program {
        std::unique_ptr<term> _head;
        std::vector<std::unique_ptr<term>> _tail;
    public:
        rule(std::unique_ptr<term> head, std::vector<std::unique_ptr<term>> tail): program(program::rule), _head(std::move(head)), _tail(std::move(tail)) {}
        static bool classof(program *p) {
            return p->kind() == program::rule;
        }
    };

    struct query {
        std::vector<std::unique_ptr<term>> terms;

        query(std::vector<std::unique_ptr<term>> terms) : terms(std::move(terms)) {}
    };

}
