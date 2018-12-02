#pragma once

#include <vector>
#include "inst.h"
#include "ast.h"

namespace prolog0 {

class codegen {

    std::vector<inst> _inst_stream;

public:
    void compile_query(const query &);

};

}
