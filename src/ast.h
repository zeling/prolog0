#pragma once

#include <ostream>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <initializer_list>
#include <assert.h>
#include <llvm/Support/Casting.h>

#include "util.h"

namespace prolog0 {

    class term {
    public:
        enum kind {
            variable,
            structure,
//            constant
        };
    private:
        const kind _kind;
    public:
        term() = default;

        term(kind k) : _kind(k) {}

        kind kind() const {
            return _kind;
        }

        virtual ~term() {}
    };

    template<typename Visitor>
    struct term_visitor;

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

        functor(std::string name, size_t arity) : name(std::move(name)), arity(arity) {}

        friend std::ostream &operator<<(std::ostream &o, const functor &f) {
            o << f.name << "/" << f.arity;
        }
    };


    struct structure : public term {
        struct functor functor;
        std::vector<std::unique_ptr<term>> args;

        structure(std::string name, size_t arity, std::vector<std::unique_ptr<term>> args) :
                term(term::structure), args(std::move(args)), functor(std::move(name), arity) {
        }

        /* a special case where the structure represents a constant */
        bool is_constant() {
            return functor.arity == 0;
        }

        static bool classof(const term *t) {
            return t->kind() == term::structure;
        }
    };

//    struct constant : public term {
//        std::string name;
//
//        constant(std::string name) : term(term::constant), name(std::move(name)) {}
//
//        static bool classof(const term *t) {
//            return t->kind() == term::constant;
//        }
//
//        template<typename Visitor>
//        void accept(term_visitor<Visitor> &v) {
//            v.visit(this);
//        }
//    };

    class program {
    public:
        enum kind {
            fact,
            rule,
        };
    private:
        const kind _kind;
    public:
        program(kind k) : _kind(k) {}

        kind kind() const {
            return _kind;
        }

        virtual ~program() {}
    };

    struct fact : public program {
        std::unique_ptr<structure> _str;
    public:
        fact(std::unique_ptr<structure> s) : program(program::fact), _str(std::move(s)) {}

        static bool classof(const program *p) {
            return p->kind() == program::fact;
        }
    };

    struct rule : public program {
        std::unique_ptr<structure> head;
        std::vector<std::unique_ptr<structure>> tail;
    public:
        rule(std::unique_ptr<structure> head, std::vector<std::unique_ptr<structure>> tail)
            : program(program::rule),
              head(std::move(head)),
              tail(std::move(tail)) {}

        static bool classof(const program *p) {
            return p->kind() == program::rule;
        }
    };

    struct query {
        std::vector<std::unique_ptr<structure>> terms;

        query(std::vector<std::unique_ptr<structure>> terms) : terms(std::move(terms)) {}
    };

    template<typename Visitor>
    struct ast_visitor {
#define DELEGATE_VISIT(what) void visit(const what *x) { static_cast<Visitor *>(this)->visit_##what(x); }

        DELEGATE_VISIT(term);
        DELEGATE_VISIT(variable);
        DELEGATE_VISIT(structure);
        DELEGATE_VISIT(query);
        DELEGATE_VISIT(program);
        DELEGATE_VISIT(rule);
        DELEGATE_VISIT(fact);

#define EMPTY_VISIT(what) void visit_##what(const what *x) {}

        // default implementations:
        void visit_term(const term *t) {
            switch (t->kind()) {
                case term::variable:
                    return visit(llvm::dyn_cast<variable>(t));
                case term::structure:
                    return visit(llvm::dyn_cast<structure>(t));
            }
        }

        EMPTY_VISIT(variable);
        EMPTY_VISIT(structure);

        void walk_preorder(const term *t) {
            visit(t);
            if (auto s = llvm::dyn_cast<structure>(t)) {
                for (auto &sub: s->args) {
                    walk_preorder(sub.get());
                }
            }
        }

        void walk_postorder(const term *t) {
            if (auto s = llvm::dyn_cast<structure>(t)) {
                for (auto &sub: s->args) {
                    walk_postorder(sub.get());
                }
            }
            visit(t);
        }

        void walk_breadth_first(const term *t) {
            std::queue<const term *> q;
            q.push(t);
            while (!q.empty()) {
                auto h = q.front();
                q.pop();
                visit(h);
                if (auto s = llvm::dyn_cast<structure>(h)) {
                    for (auto &sub: s->args) {
                        q.push(sub.get());
                    }
                }
            }
        }

        EMPTY_VISIT(query);
        void visit_program(const program *p) {
            switch (p->kind()) {
                case program::fact:
                    return visit(llvm::dyn_cast<fact>(p));
                case program::rule:
                    return visit(llvm::dyn_cast<rule>(p));
            }
        }
        EMPTY_VISIT(rule);
        EMPTY_VISIT(fact);
    };

}
