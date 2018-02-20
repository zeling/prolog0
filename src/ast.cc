#include <iostream>
#include "ast.h"

term make_atom(std::string name) {
    return term(term::atom_marker, std::move(name));
}

term make_variable(std::string name) {
    return term(term::variable_marker, std::move(name));
}

std::ostream &operator<<(std::ostream &os, const term &t) {
    switch (t._kind) {
        case term::variable:
        case term::atom:
            os << t._name;
            break;
        case term::structure:
            os << t._name << '(';
            for (int i = 0; i < t._args.size() - 1; ++i) {
                os << t._args[i] << ',';
            }
            if (!t._args.empty()) {
                os << t._args.back();
            }
            os << ')';
    }
    return os;
}

term make_structure(std::string name, std::vector<term> args) {
    return term(std::move(name), std::move(args));
}

