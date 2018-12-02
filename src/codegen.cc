//
// Created by 冯泽灵 on 2018/2/20.
//

#include "codegen.h"
#include "inst.h"

struct AReg;
struct XReg;

template <typename RegTag>
inst::wam_reg_t alloc_reg() {
    static inst::wam_reg_t next_reg = 0;
    return next_reg++;
}

