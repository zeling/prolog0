#pragma once

#include <vector>
#include "inst.h"
#include "ast.h"
#include "llvm/Support/Casting.h"

namespace prolog0 {

class codegen {

    std::vector<std::unique_ptr<inst>> _inst_stream;

public:
    /* compiles to a heap repr and return the register that holds the heap repr */
    wam_reg_t compile_query(const query &);
    wam_reg_t compile_structure(const structure &);

};

struct term_hash {
    size_t operator()(term *t) const noexcept;
};

struct term_equal {
    bool operator()(const term *lhs, const term *rhs) const noexcept;
};

using reg_map_t = std::unordered_map<term *, wam_reg_t, term_hash, term_equal>;
void alloc_reg(term *trm, reg_map_t &rmap, wam_reg_t &cur, bool top_level = false);

}
