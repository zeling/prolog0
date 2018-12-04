#pragma once

#include "ast.h"
#include <iostream>
#include <unordered_map>
#include <sstream>

namespace prolog0 {

#define INST_LIST(T) \
  T(get_structure)   \
  T(put_structure)   \
  T(put_value)       \
  T(get_value)       \
  T(set_value)       \
  T(put_variable)    \
  T(get_variable)    \
  T(set_variable)    \
  T(call)            \

using wam_reg_t = uintptr_t;
using wam_addr_t = uintptr_t;
using wam_functor_t = size_t;

class inst {

protected:
    const char *get_inst_name() const;
public:
    enum kind {
#define T(e) e,
        INST_LIST(T)
#undef T
        NUM_INST
    } kind;

    inst(enum kind kind): kind(kind) {}

    virtual std::string to_string() const = 0;

    friend std::ostream &operator<<(std::ostream &, const inst &);
};

class put_structure : public inst {
    wam_functor_t _fname;
    size_t _arity;
    wam_reg_t _x;

    static std::vector<std::string> _functors;
    static std::unordered_map<std::string, wam_functor_t> _mapping;
    static wam_functor_t from_string(const std::string &name);

public:
    put_structure(const functor &f, wam_reg_t reg):
        inst(inst::put_structure), _fname(from_string(f.name)), _arity(f.arity), _x(reg) {}

    std::string to_string() const override {
        std::ostringstream ss;
        ss << get_inst_name() << " " << _functors[_fname] << "/" << _arity << " X" << _x;
        return ss.str();
    }

    static bool classof(const inst *i) {
        return i->kind == inst::put_structure;
    }
};


class put_variable : public inst {
    wam_reg_t _x;
    wam_reg_t _a;

public:
    put_variable(wam_reg_t x, wam_reg_t a): inst(inst::put_variable), _x(x), _a(a) {}

    std::string to_string() const override {
        std::ostringstream ss;
        ss << get_inst_name() << " X" << _x << " A" << _a;
        return ss.str();
    }

    static bool classof(const inst *i) {
        return i->kind == inst::put_variable;
    }
};

class put_value : public inst {
    wam_reg_t _x;
    wam_reg_t _a;

public:
    put_value(wam_reg_t x, wam_reg_t a): inst(inst::put_value), _x(x), _a(a) {}
    std::string to_string() const override {
        std::ostringstream ss;
        ss << get_inst_name() << " X" << _x << " A" << _a;
        return ss.str();
    }
    static bool classof(const inst *i) {
        return i->kind == inst::put_value;
    }
};

class set_value : public inst {
    wam_reg_t x;
public:
    set_value(wam_reg_t x): inst(inst::set_value), x(x) {}
    std::string to_string() const override {
        std::ostringstream ss;
        ss << get_inst_name() << " X" << x;
        return ss.str();
    }
    static bool classof(const inst *i) {
        return i->kind == inst::set_value;
    }
};

class set_variable : public inst {
    wam_reg_t x;
public:
    set_variable(wam_reg_t x): inst(inst::set_variable), x(x) {}
    std::string to_string() const override {
        std::ostringstream ss;
        ss << get_inst_name() << " X" << x;
        return ss.str();
    }

    static bool classof(const inst *i) {
        return i->kind == inst::set_variable;
    }
};

class call : public inst {
    functor f;

public:
    call(functor f): inst(inst::call), f(std::move(f)) {}
    std::string to_string() const override {
        std::ostringstream ss;
        ss << get_inst_name() << " " << f.name << "/" << f.arity;
        return ss.str();
    }

    static bool classof(const inst *i) {
        return i->kind == inst::call;
    }
};


}
