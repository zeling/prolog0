#include <unordered_map>
#include "codegen.h"
#include "inst.h"
#include "llvm/Support/Casting.h"

namespace prolog0 {

struct AReg;
struct XReg;

template<typename RegTag>
wam_reg_t alloc_reg() {
    static wam_reg_t next_reg = 0;
    return next_reg++;
}

static size_t hash_combine( size_t lhs, size_t rhs ) {
    lhs^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}


void alloc_reg(term *trm, reg_map_t &rmap, wam_reg_t &cur, uint32_t level) {
    if (level == 0) {
        if (auto str = llvm::dyn_cast<structure>(trm)) {
            cur = str->args.size() + 1;
            for (auto &arg : str->args) {
                alloc_reg(arg.get(), rmap, cur, level + 1);
            }
        }
        return;
    }

    /* the trm has already seen */
    if (rmap.find(trm) != rmap.end()) {
        return;
    }

    if (level > 1 || trm->kind() == term::variable) rmap[trm] = cur++;
    if (auto str = llvm::dyn_cast<structure>(trm)) {
        for (auto &arg : str->args) {
            alloc_reg(arg.get(), rmap, cur, level + 1);
        }
    }
}

reg_map_t alloc_reg(term *trm) {
    reg_map_t ret;
    wam_reg_t cur = 0;
    alloc_reg(trm, ret, cur, 0);
    return ret;
}

void codegen::compile_query(const query *qry) {
    /* only one clause for now */
    auto cls = qry->terms[0].get();
    auto reg_map = alloc_reg(cls);
    std::unordered_set<const term *, term_hash, term_equal> var_seen;

    auto rmap = alloc_reg(cls);
    if (auto str = llvm::dyn_cast<structure>(cls)) {
        size_t arg_pos = 1;
        for (auto &arg : str->args) {
            compile_term(arg.get(), rmap, arg_pos++, var_seen);
        }
        _inst_stream.emplace_back(new call(str->functor));
    } else if (auto var = llvm::dyn_cast<variable>(cls)) {
        _inst_stream.emplace_back(new put_variable(rmap[var], 1));
    } else if (auto cst = llvm::dyn_cast<constant>(cls)) {
        _inst_stream.emplace_back(new put_structure(functor(cst->name, 0), 0));
    }
}

void codegen::compile_term(const term *t, reg_map_t &rmap, size_t arg_pos) {
    if (auto str = llvm::dyn_cast<structure>(t)) {
        _inst_stream.emplace_back(new put_structure(str->functor, arg_pos ? arg_pos : rmap[str]));
        for (auto &arg : str->args) {
            auto *parg = arg.get();
            compile_term(parg, rmap, 0);
        }
    } else if (auto cst = llvm::dyn_cast<constant>(t)) {
        _inst_stream.emplace_back(new put_structure(functor(cst->name, 0), rmap[cst]));
    } else if (auto var = llvm::dyn_cast<variable>(t)) {
        if (arg_pos) {
            _inst_stream.emplace_back(new put_variable(rmap[var], arg_pos));
        } else {
            _inst_stream.emplace_back(new set_variable(rmap[var]));
        }
    }
}

void codegen::compile_term(const term *t, reg_map_t &rmap, size_t arg_pos, std::unordered_set<const term *, term_hash, term_equal> &var_seen) {
    if (auto str = llvm::dyn_cast<structure>(t)) {
        _inst_stream.emplace_back(new put_structure(str->functor, arg_pos ? arg_pos : rmap[str]));
        for (auto &arg : str->args) {
            auto *parg = arg.get();
            compile_term(parg, rmap, 0, var_seen);
        }
    } else if (auto cst = llvm::dyn_cast<constant>(t)) {
        _inst_stream.emplace_back(new put_structure(functor(cst->name, 0), rmap[cst]));
    } else if (auto var = llvm::dyn_cast<variable>(t)) {
        if (arg_pos) {
            _inst_stream.emplace_back(new put_variable(rmap[var], arg_pos));
        } else {
            if (var_seen.find(var) == var_seen.end()) {
                _inst_stream.emplace_back(new set_variable(rmap[var]));
            } else {
                _inst_stream.emplace_back(new set_value(rmap[var]));
            }
        }
        var_seen.insert(var);
    }

}

void codegen::print_to_stream(std::ostream &s) {
    for (auto &i : _inst_stream) {
        s << *i << std::endl;
    }

}


//void codegen::compile_term(const tertruem *t, reg_map_t &rmap, wam_reg_t r, bool top_level) {
//    if (auto str = llvm::dyn_cast<structure>(t)) {
//        wam_reg_t reg = top_level ? r : rmap[str];
//        _inst_stream.emplace_back(new put_structure(str->functor, reg));
//        size_t nr_args = str->args.size();
//        for (auto &arg : str->args) {
//            auto *parg = arg.get();
//            if (top_level) {
//                compile_term(parg, rmap, nr_args + 1, false);
//            } else {
//                compile_term(parg, rmap, r + 1, false);
//            }
//        };
//    } else if (auto cst = llvm::dyn_cast<constant>(t)) {
//        _inst_stream.emplace_back(new put_structure(functor(cst->name, 0), rmap[cst]));
//    } else if (auto var = llvm::dyn_cast<variable>(t)) {
//        if (rmap.find(var) == rmap.end()) {
//            if (top_level) {
//                _inst_stream.emplace_back(new put_variable(rmap[var], r));
//            } else {
//                _inst_stream.emplace_back(new set_variable(r));
//            }
//        } else {
//
//        }
//        if (arg_pos) {
//            _inst_stream.emplace_back(new put_variable(rmap[var], arg_pos));
//        } else {
//            _inst_stream.emplace_back(new set_variable(rmap[var]));
//        }
//    }
//
//}

size_t term_hash::operator()(const term *t) const noexcept {
    if (auto str = llvm::dyn_cast<structure>(t)) {
        std::hash<std::string> sh;
        size_t ret = sh(str->functor.name);
        for (auto &arg : str->args) {
            term_hash h;
            ret = hash_combine(ret, h(arg.get()));
        }
        return ret;
    } else if (auto v = llvm::dyn_cast<variable>(t)) {
        std::hash<std::string> h;
        return h(v->name);
    } else {
        std::hash<std::string> h;
        return h(v->name);
    }
}

bool term_equal::operator()(const term *lhs, const term *rhs) const noexcept {
    if (lhs->kind() != rhs->kind()) return false;
    if (auto l_str = llvm::dyn_cast<structure>(lhs)) {
        auto r_str = llvm::dyn_cast<structure>(rhs);
        if (l_str->args.size() != r_str->args.size()) {
            return false;
        } else {
            term_equal te;
            for (int i = 0; i < l_str->args.size(); i++) {
                if (!te(l_str->args[i].get(), r_str->args[i].get())) {
                    return false;
                }
            }
            return true;
        }
    } else if (auto l_v = llvm::dyn_cast<variable>(lhs)) {
        auto r_v = llvm::dyn_cast<variable>(rhs);
        return l_v->name == r_v->name;
    } else if (auto l_c = llvm::dyn_cast<constant>(lhs)) {
        auto r_c = llvm::dyn_cast<constant>(rhs);
        return l_c->name == r_c->name;
    } else {
        assert(0);
    }
}

}

