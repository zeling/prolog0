#pragma once

#include <vector>
#include "inst.h"
#include "ast.h"


class codegen {

    std::vector<inst> _inst_stream;

public:
    void compile_query(const query &);

};
