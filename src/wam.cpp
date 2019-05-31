#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <queue>

#include "ast.h"
#include "inst.h"
#include "llvm/Support/Casting.h"

namespace prolog0 {

    using inst_stream = std::vector<inst *>;

#define EMIT_INST(name, ...) _o.push_back(new name(__VA_ARGS__))

    struct reg_alloc {
        std::unordered_map<term *, reg_t> _reg_alloc;

        void map(term *t, reg_t r) {
            _reg_alloc[t] = r;
        }

        reg_t get(term *t) {
            auto it = _reg_alloc.find(t);
            assert(it != _reg_alloc.end());
            return it->second;
        }
    };

    template <typename Compile>
    struct compile_visitor: ast_visitor<compile_visitor<Compile>> {
        std::unordered_map<term *, reg_t> _flattened;
        std::unordered_set<std::string> _varseen;
        inst_stream &_o;
        compile_visitor(std::unordered_map<term *, reg_t> f, inst_stream &o): _flattened(std::move(f)), _o(o) {}

        void visit_structure(structure *s) {
            static_cast<Compile *>(this)->emit_structure(s);
            for (auto &t: s->args) {
                if (auto v = llvm::dyn_cast<variable>(t.get())) {
                    if (_varseen.find(v->name) == _varseen.end()) {
                        _varseen.insert(v->name);
                        static_cast<Compile *>(this)->emit_nonseen_variable(v);
                    } else {
                        static_cast<Compile *>(this)->emit_seen_variable(v);
                    }
                } else {
                    static_cast<Compile *>(this)->emit_nonseen_variable(t.get());
                }
            }
        }

        void compile(structure *s) {
            for (auto &t: s->args) {
                if (auto v = llvm::dyn_cast<variable>(t.get())) {
                    if (_varseen.find(v->name) == _varseen.end()) {
                        _varseen.insert(v->name);
                        static_cast<Compile *>(this)->emit_nonseen_argument(v);
                    } else {
                        static_cast<Compile *>(this)->emit_seen_argument(v);
                    }
                } else {
                    static_cast<Compile *>(this)->emit_structure_argument(t.get());
                }
            }
            static_cast<Compile *>(this)->emit_toplevel(s);
        }
    };

    std::unordered_map<term *, reg_t> flatten(structure *s) {
        std::queue<term *> q;
        struct visitor: ast_visitor<visitor> {
            register_t reg = 0;
            std::unordered_map<term *, reg_t> flattened;
            std::unordered_set<std::string> varseen;
            std::queue<term *> &q = q;

            void visit_variable(variable *v) {
                if (varseen.find(v->name) != varseen.end()) {
                    flattened[v] = reg++;
                }
            }

            void visit_structure(structure *s) {
                flattened[s] = reg++;
                for (auto &sub: s->args) {
                    q.push(sub.get());
                }
            }
        } v;
        q.push(s);
        while (!q.empty()) {
            auto t = q.front();
            q.pop();
            v.visit(t);
        }
        return v.flattened;
    }

    void compile_query(inst_stream &o, const query &q) {
        auto flattened = flatten(q.terms[0].get());
        struct compile: compile_visitor<compile> {
            compile(std::unordered_map<term *, reg_t> f, inst_stream &o): compile_visitor(std::move(f), o) {}
            void emit_structure(structure *s) {
                EMIT_INST(put_structure, s->functor, _flattened[s]);
            }

            void emit_nonseen_variable(variable *v) {
                EMIT_INST(set_variable, _flattened[v]);
            }

            void emit_seen_variable(variable *v) {
                EMIT_INST(set_value, _flattened[v]);
            }

            void emit_nonseen_argument(term *t, reg_t arg) {
                EMIT_INST(put_value, _flattened[t], arg);
            }

            void emit_seen_argument(term *t, reg_t arg) {
                EMIT_INST(put_value, _flattened[t], arg);
            }

            void emit_toplevel(structure *s) {
                EMIT_INST(call, s->functor);
            }
        } c{std::move(flattened), o};
        c.walk_structure_postorder(q.terms[0].get());
    }

    void compile_fact(inst_stream &o, const fact &f) {
        auto flattend = flatten(f._str.get());
//        compile_visitor v(std::move(flattend), o);
//        v.walk_structure_preorder(f._str.get());
    }

}


