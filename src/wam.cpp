#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <iostream>

#include "ast.h"
#include "inst.h"
#include "llvm/Support/Casting.h"

namespace prolog0 {

    using inst_stream = std::vector<inst *>;

#define EMIT_INST(name, ...) _o.push_back(new name(__VA_ARGS__))

    template <typename Compile>
    struct compile_visitor: ast_visitor<compile_visitor<Compile>> {
        std::unordered_set<std::string> _varseen;
        std::unordered_map<const term *, reg_t> _args;
        inst_stream &_o;
        const structure *_top;
        compile_visitor(inst_stream &o, const structure *top): _top(top), _o(o) {
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


        void toplevel(const structure *s) {
            static_cast<Compile *>(this)->emit_toplevel(s);
        }
    };

    std::unordered_map<const term *, reg_t> flatten(const structure *s) {
        reg_t start = s->args.size();
        struct visitor: ast_visitor<visitor> {
            reg_t _reg;
            std::unordered_map<const term *, reg_t> _flattened;
            std::unordered_map<std::string, const variable *> _varseen;

            visitor(reg_t start, const structure *s): _reg(start) {
                reg_t arg = 1;
                for (auto &t: s->args) {
                    if (auto s = llvm::dyn_cast<structure>(t.get())) {
                        _flattened[s] = arg;
                    }
                    arg++;
                }
            }

            void visit_variable(const variable *v) {
                if (_varseen.find(v->name) == _varseen.end()) {
                    _flattened[v] = _reg++;
                    _varseen[v->name] = v;
                } else {
                    _flattened[v] = _flattened[_varseen[v->name]];
                }
            }

            void visit_structure(const structure *s) {
                if (_flattened.find(s) == _flattened.end()) {
                    _flattened[s] = _reg++;
                }
            }
        } v {start, s};
        v.walk_preorder(s);
        return v._flattened;
    }

    void compile_query(inst_stream &o, const structure *s) {
        auto flattened = flatten(s);
        struct compile: compile_visitor<compile> {
            std::unordered_map<const term *, reg_t> _flattened;
            compile(std::unordered_map<const term *, reg_t> f, inst_stream &o, const structure *s): compile_visitor(o, s), _flattened(std::move(f)) {}
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
        } c{std::move(flattened), o, s};
        c.walk_preorder(s);
        c.emit_toplevel(s);
    }

    void compile_fact(inst_stream &o, const structure *s) {
        auto flattened = flatten(s);
        struct compile: compile_visitor<compile> {
            std::unordered_set<const variable *> _generated;
            std::unordered_map<const term *, reg_t> _flattened;
            compile(std::unordered_map<const term *, reg_t> f, inst_stream &o, const structure *s): compile_visitor(o, s), _flattened(std::move(f)) {}
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
        } c{std::move(flattened), o, s};
        c.walk_breadth_first(s);
        c.emit_toplevel(s);
    }

}


