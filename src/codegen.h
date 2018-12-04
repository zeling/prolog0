#pragma once

#include <vector>
#include <unordered_set>
#include "inst.h"
#include "ast.h"
#include "llvm/Support/Casting.h"

namespace prolog0 {

struct term_hash {
    size_t operator()(const term *t) const noexcept;
};

struct term_equal {
    bool operator()(const term *lhs, const term *rhs) const noexcept;
};

using reg_map_t = std::unordered_map<const term *, wam_reg_t, term_hash, term_equal>;
void alloc_reg(term *trm, reg_map_t &rmap, wam_reg_t &cur, uint32_t level = 0);

class codegen {

    std::vector<std::unique_ptr<inst>> _inst_stream;

public:
    /* compiles to a heap repr and return the register that holds the heap repr */
    void compile_query(const query *);


    void compile_term(const term *, reg_map_t &rmap, size_t arg_pos = 0);
    void compile_term(const term *, reg_map_t &rmap, size_t arg_pos, std::unordered_set<const term *, term_hash, term_equal> &var_seen);


    void print_to_stream(std::ostream &s);
};


}
