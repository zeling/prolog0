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


void alloc_reg(term *trm, reg_map_t &rmap, wam_reg_t &cur, bool top_level) {
    if (top_level) {
        if (auto str = llvm::dyn_cast<structure>(trm)) {
            cur = str->args.size() + 1;
            for (auto &arg : str->args) {
                alloc_reg(arg.get(), rmap, cur);
            }
        }
        return;
    }

    /* the trm has already seen */
    if (rmap.find(trm) != rmap.end()) {
        return;
    }

    rmap[trm] = cur++;
    if (auto str = llvm::dyn_cast<structure>(trm)) {
        for (auto &arg : str->args) {
            alloc_reg(arg.get(), rmap, cur);
        }
    }
}


wam_reg_t codegen::compile_query(const query &qry) {
    /* only one clause for now */
    auto cls = qry.terms[0].get();

    if (auto str = llvm::dyn_cast<structure>(cls)) {
        for (auto &arg : str->args) {

        }
    }
}

    wam_reg_t codegen::compile_structure(const structure &s) {
        _inst_stream.emplace_back(new put_structure(s.functor, alloc_reg<AReg>()));
        for (auto &arg : s.args) {
            auto *parg = arg.get();
            if (auto c = llvm::dyn_cast<constant>(parg)) {
                /* for now, there will be an optimization in L4 */
                _inst_stream.emplace_back(new put_structure(functor(c->name, 0), alloc_reg<AReg>()));
            } else if (auto s = llvm::dyn_cast<structure>(parg)) {

            } else if (auto v = llvm::dyn_cast<variable>(parg)) {
            }
        }
    }

    size_t term_hash::operator()(term *t) const noexcept {
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

