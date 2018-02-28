#include <iostream>
#include "ast.h"

term make_atom(std::string) { return {}; }
term make_variable(std::string) { return {}; }
term make_structure(std::string, std::vector<term>) { return {}; }
