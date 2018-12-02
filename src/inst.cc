#include <iostream>
#include "inst.h"



namespace prolog0 {

const char *inst::get_inst_name() const {
    static const char *inst_names[] = {
#define T(e) #e,
            INST_LIST(T)
#undef T
    };
    return inst_names[kind];
}

std::ostream &operator<<(std::ostream &os, const inst &i) {
    os << i.to_string();
    return os;
}

std::vector<std::string> put_structure::_functors;
std::unordered_map<std::string, wam_functor_t> put_structure::_mapping;

wam_functor_t put_structure::from_string(const std::string &name) {
    auto it = _mapping.find(name);
    if (it != _mapping.end()) {
        return it->second;
    } else {
        wam_functor_t now = _mapping.size();
        _mapping[name] = now;
        _functors.push_back(name);
        return now;
    }
}
}
