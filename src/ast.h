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

    class ast {
    public:
        enum kind {
            query,
            program,
        };
    private:
        const kind _kind;
    public:
        ast(kind k): _kind(k) {}

        kind kind() const {
            return _kind;
        }

        virtual ~ast() {}
    };

    class program : ast {
    public:
        enum kind {
            fact,
            rule,
        };
    private:
        const kind _kind;
    public:
        program(kind k) : ast(ast::program), _kind(k) {}

        kind kind() const {
            return _kind;
        }

        virtual ~program() {}

        static bool classof(const ast *a) {
            return a->kind() == ast::program;
        }
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

    struct query: ast {
        std::vector<std::unique_ptr<structure>> terms;
        query(std::vector<std::unique_ptr<structure>> terms): ast(ast::query), terms(std::move(terms)) {}

        static bool classof(const ast *a) {
            return a->kind() == ast::query;
        }
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
        DELEGATE_VISIT(ast);

#define EMPTY_VISIT(what) void visit_##what(const what *x) {}

        // default implementations:
        void visit_term(const term *t) {
            switch (t->kind()) {
                case term::variable:
                    return visit(llvm::cast<variable>(t));
                case term::structure:
                    return visit(llvm::cast<structure>(t));
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
                    return visit(llvm::cast<fact>(p));
                case program::rule:
                    return visit(llvm::cast<rule>(p));
            }
        }
        EMPTY_VISIT(rule);
        EMPTY_VISIT(fact);

        void visit_ast(const ast *a) {
            switch (a->kind()) {
                case ast::program:
                    return visit(llvm::cast<program>(a));
                case ast::query:
                    return visit(llvm::cast<query>(a));
            }
        }
    };

}
