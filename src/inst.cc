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

std::vector<std::string> functors;
std::unordered_map<std::string, wam_functor_t> mapping;

wam_functor_t from_string(const std::string &name) {
    auto it = mapping.find(name);
    if (it != mapping.end()) {
        return it->second;
    } else {
        wam_functor_t now = mapping.size();
        mapping[name] = now;
        functors.push_back(name);
        return now;
    }
}

std::string put_structure::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " " << functors[_fname] << "/" << _arity << " X" << _x;
    return ss.str();
}

std::string put_variable::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " X" << _x << " A" << _a;
    return ss.str();
}

std::string put_value::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " X" << _x << " A" << _a;
    return ss.str();
}

std::string set_value::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " X" << x;
    return ss.str();
}

std::string set_variable::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " X" << x;
    return ss.str();
}

std::string call::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " " << f.name << "/" << f.arity;
    return ss.str();
}

std::string get_structure::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " " << functors[_fname] << "/" << _arity << " X" << _x;
    return ss.str();
}

std::string unify_variable::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " X" << _x;
    return ss.str();
}

std::string get_value::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " X" << _x << " A" << _a;
    return ss.str();
}

std::string get_variable::to_string() const {
    std::ostringstream ss;
    ss << get_inst_name() << " X" << _x << " A" << _a;
    return ss.str();
}

}
