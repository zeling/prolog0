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
  T(unify_value)     \
  T(put_variable)    \
  T(get_variable)    \
  T(unify_variable)  \
  T(set_variable)    \
  T(call)            \
  T(proceed)         \

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

    virtual ~inst() {}
};

wam_functor_t from_string(const std::string &name);

class put_structure : public inst {
    wam_functor_t _fname;
    size_t _arity;
    wam_reg_t _x;

public:
    put_structure(const functor &f, wam_reg_t reg):
        inst(inst::put_structure), _fname(from_string(f.name)), _arity(f.arity), _x(reg) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::put_structure;
    }
};


class put_variable : public inst {
    wam_reg_t _x;
    wam_reg_t _a;

public:
    put_variable(wam_reg_t x, wam_reg_t a): inst(inst::put_variable), _x(x), _a(a) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::put_variable;
    }
};

class put_value : public inst {
    wam_reg_t _x;
    wam_reg_t _a;

public:
    put_value(wam_reg_t x, wam_reg_t a): inst(inst::put_value), _x(x), _a(a) {}
    std::string to_string() const override;
    static bool classof(const inst *i) {
        return i->kind == inst::put_value;
    }
};

class set_value : public inst {
    wam_reg_t x;
public:
    set_value(wam_reg_t x): inst(inst::set_value), x(x) {}
    std::string to_string() const override;
    static bool classof(const inst *i) {
        return i->kind == inst::set_value;
    }
};

class set_variable : public inst {
    wam_reg_t x;
public:
    set_variable(wam_reg_t x): inst(inst::set_variable), x(x) {}
    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::set_variable;
    }
};

class call : public inst {
    functor f;

public:
    call(functor f): inst(inst::call), f(std::move(f)) {}
    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::call;
    }
};

class get_structure : public inst {
    wam_functor_t _fname;
    size_t _arity;
    wam_reg_t _x;

public:
    get_structure(const functor &f, wam_reg_t reg): inst(inst::get_structure), _fname(from_string(f.name)), _arity(f.arity), _x(reg) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::get_structure;
    }

};

class unify_variable : public inst {
    wam_reg_t _x;

public:
    unify_variable(wam_reg_t x): inst(inst::unify_variable), _x(x) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::unify_variable;
    }
};

class get_value : public inst {
    wam_reg_t _x;
    wam_reg_t _a;

public:
    get_value(wam_reg_t x, wam_reg_t a): inst(inst::get_value), _x(x), _a(a) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::get_value;
    }

};

class get_variable : public inst {
    wam_reg_t _x;
    wam_reg_t _a;
public:
    get_variable(wam_reg_t x, wam_reg_t a): inst(inst::get_variable), _x(x), _a(a) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::get_variable;
    }
};

class unify_value : public inst {
    wam_reg_t _x;
public:
    unify_value(wam_reg_t x): inst(inst::unify_value), _x(x) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::unify_value;
    }
};

class proceed : public inst {
public:
    proceed(): inst(inst::proceed) {}

    std::string to_string() const override;

    static bool classof(const inst *i) {
        return i->kind == inst::proceed;
    }
};


}
