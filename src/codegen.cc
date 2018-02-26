//
// Created by 冯泽灵 on 2018/2/20.
//

#include "codegen.h"

void codegen::compile_query(const query &q) {
    auto &t = q.term();
    switch (t.kind) {
        case term::variable:
            _inst_stream.emplace_back(inst::set_variable);
            break;
        case term::atom:
            _inst_stream.emplace_back(inst::set_value);
            break;
        case term::structure:
            _inst_stream.emplace_back(inst::put_structure);
    }
}
