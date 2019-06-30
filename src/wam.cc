#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <algorithm>

#include "ast.h"
#include "inst.h"
#include "llvm/Support/Casting.h"
#include "wam.h"

namespace prolog0 {

    template <typename T>
    using varmap = std::unordered_map<std::string, T>;

    template<typename T>
    using termmap = std::unordered_map<const term *, T>;
#define EMIT_INST(name, ...) _o.push_back(new name(__VA_ARGS__))

    template <typename Compile>
    struct compile_visitor: ast_visitor<compile_visitor<Compile>> {
        varset _varseen;
        termmap<reg_t> _args;
        inst_stream &_o;
        const structure *_top;
        compile_visitor(inst_stream &o, const structure *top, varset varseen): _top(top), _o(o), _varseen(std::move(varseen)) {
            reg_t arg = 1;
            for (auto &t: top->args) {
                _args[t.get()] = arg++;
            }
        }

        void visit_structure(const structure *s) {
            if (s != _top) {
                static_cast<Compile *>(this)->emit_structure(s);
            }
        }

        void visit_variable(const variable *v) {
            bool seen = _varseen.find(v->name) != _varseen.end();
            if (_args.find(v) != _args.end()) {
                if (seen) static_cast<Compile *>(this)->emit_seen_variable_argument(v, _args[v]);
                else static_cast<Compile *>(this)->emit_nonseen_variable_argument(v, _args[v]);
            } else {
                if (seen) static_cast<Compile *>(this)->emit_seen_variable(v);
                else static_cast<Compile *>(this)->emit_nonseen_variable(v);
            }
            if (!seen) _varseen.insert(v->name);
        }
    };

    termmap<reg_t> flatten(const structure *s, varmap<reg_t> *env) {
        reg_t start = s->args.size();
        struct visitor: ast_visitor<visitor> {
            reg_t _x_reg;
            varmap<reg_t> *_env;
            termmap<reg_t> _flattened;

            visitor(reg_t start, const structure *s, varmap<reg_t> *env): _x_reg(start), _env(env) {
                reg_t arg = 1;
                for (auto &t: s->args) {
                    if (auto s = llvm::dyn_cast<structure>(t.get())) {
                        _flattened[s] = arg;
                    }
                    arg++;
                }
            }

            void visit_variable(const variable *v) {
                auto it = _env->find(v->name);
                if (it != _env->end()) {
                    _flattened[v] = it->second;
                } else {
                    _flattened[v] = _x_reg++;
                }
            }

            void visit_structure(const structure *s) {
                if (_flattened.find(s) == _flattened.end()) {
                    _flattened[s] = _x_reg++;
                }
            }
        } v {start, s, env};
        v.walk_preorder(s);
        return v._flattened;
    }

    varset compile_query_term(inst_stream &_o, const structure *s, varset seen, varmap<reg_t> *env) {
        auto flattened = flatten(s, env);
        struct compile: compile_visitor<compile> {
            std::unordered_map<const term *, reg_t> _flattened;
            compile(termmap<reg_t> f, inst_stream &o, const structure *s, varset seen): compile_visitor(o, s, std::move(seen)), _flattened(std::move(f)) {}
            void emit_structure(const structure *s) {
                EMIT_INST(put_structure, s->functor, _flattened[s]);
            }
            void emit_nonseen_variable(const variable *v) {
                EMIT_INST(set_variable, _flattened[v]);
            }
            void emit_seen_variable(const variable *v) {
                EMIT_INST(set_value, _flattened[v]);
            }
            void emit_nonseen_variable_argument(const term *t, reg_t arg) {
                EMIT_INST(put_variable, _flattened[t], arg);
            }
            void emit_seen_variable_argument(const term *t, reg_t arg) {
                EMIT_INST(put_value, _flattened[t], arg);
            }
            void emit_toplevel(const structure *s) {
                EMIT_INST(call, s->functor);
            }
        } c{std::move(flattened), _o, s, seen};
        c.walk_preorder(s);
        EMIT_INST(call, s->functor);
        return c._varseen;
    }

    varset compile_program_term(inst_stream &o, const structure *s, varset seen, varmap<reg_t> *env) {
        auto flattened = flatten(s, env);
        struct compile: compile_visitor<compile> {
            std::unordered_set<const variable *> _generated;
            std::unordered_map<const term *, reg_t> _flattened;
            compile(std::unordered_map<const term *, reg_t> f, inst_stream &o, const structure *s, varset seen)
                    : compile_visitor(o, s, seen), _flattened(std::move(f)) {}
            void emit_structure(const structure *s) {
                EMIT_INST(get_structure, s->functor, _flattened[s]);
                for (auto &arg: s->args) {
                    reg_t reg = _flattened[arg.get()];
                    if (auto v = llvm::dyn_cast<variable>(arg.get())) {
                        if (_varseen.find(v->name) == _varseen.end()) {
                            EMIT_INST(unify_variable, reg);
                            _varseen.insert(v->name);
                        } else {
                            EMIT_INST(unify_value, reg);
                        }
                        _generated.insert(v);
                    } else {
                        EMIT_INST(unify_variable, reg);
                    }
                }
            }
            void emit_nonseen_variable(const variable *v) {
                if (_generated.find(v) == _generated.end()) {
                    EMIT_INST(unify_variable, _flattened[v]);
                    _generated.insert(v);
                }
            }
            void emit_seen_variable(const variable *v) {
                if (_generated.find(v) == _generated.end()) {
                    EMIT_INST(unify_value, _flattened[v]);
                    _generated.insert(v);
                }
            }
            void emit_nonseen_variable_argument(const term *t, reg_t arg) {
                EMIT_INST(get_variable, _flattened[t], arg);
            }
            void emit_seen_variable_argument(const term *t, reg_t arg) {
                EMIT_INST(get_value, _flattened[t], arg);
            }
            void emit_toplevel(const structure *s) {
                EMIT_INST(proceed);
            }
        } c{std::move(flattened), o, s, std::move(seen)};
        c.walk_breadth_first(s);
        return std::move(c._varseen);
    }

    std::unordered_set<std::string> get_names(const term *t) {
        struct visitor: ast_visitor<visitor> {
            std::unordered_set<std::string> _met;
            void visit_variable(const variable *v) {
                _met.insert(v->name);
            }
        } v;
        v.walk_preorder(t);
        return v._met;
    }

    template <typename Iterator>
    varmap<reg_t> analyze_environment(varset first, Iterator begin, Iterator end) {
        varmap<reg_t> out;
        for (auto it = begin; it != end; ++it) {
            auto m = get_names(it->get());
            for (const auto &v: m) {
                if (first.find(v) != first.end()) {
                    out[v] = -out.size() - 1;
                }
                first.insert(v);
            }
        }
        return out;
    }

    void compile_query(inst_stream &_o, const query *qry) {
        assert(qry->terms.size() > 0);
        auto first = get_names(qry->terms[0].get());
        auto env = analyze_environment(std::move(first), ++qry->terms.begin(), qry->terms.end());
        size_t env_size = env.size();
        if (env_size) EMIT_INST(allocate, env_size);
        varset seen;
        for (auto &t: qry->terms) {
            seen = compile_query_term(_o, t.get(), std::move(seen), &env);
        }
        if (env_size) EMIT_INST(deallocate);
    }

    void compile_fact(inst_stream &_o, const fact *f) {
        varmap<reg_t> empty_env;
        compile_program_term(_o, f->_str.get(), {}, &empty_env);
        EMIT_INST(proceed);
    }

    void compile_rule(inst_stream &_o, const rule *r) {
        assert(r->tail.size() > 0);
        auto first = get_names(r->head.get());
        for (auto v: get_names(r->tail[0].get())) {
            first.insert(v);
        }

        auto env = analyze_environment(std::move(first), ++r->tail.begin(), r->tail.end());
        auto env_size = env.size();
        varset seen;
        varmap<reg_t> vars;
        if (env_size) EMIT_INST(allocate, env_size);
        seen = compile_program_term(_o, r->head.get(), std::move(seen), &env);
        for (auto &t: r->tail) {
            seen = compile_query_term(_o, t.get(), std::move(seen), &env);
        }
        if (env_size) EMIT_INST(deallocate);
    }


    void compile_program(inst_stream &o, const program *prg) {
        struct visitor: ast_visitor<visitor> {
            inst_stream &_o;
            visitor(inst_stream &o): _o(o) {}
            void visit_rule(const rule *r) {
                compile_rule(_o, r);
            }

            void visit_fact(const fact *f) {
                compile_fact(_o, f);
            }
        } v{o};
        v.visit(prg);
    }

}




