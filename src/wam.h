#pragma once
#include "inst.h"
#include <unordered_set>

namespace prolog0 {
using inst_stream = std::vector<inst *>;
using varset = std::unordered_set<std::string>;
void compile_query(inst_stream &_o, const query *qry);
void compile_program(inst_stream &_o, const program *prg);
}