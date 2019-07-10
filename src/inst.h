#pragma once
#include "ast.h"
#include <ostream>

#define DEFINE_INST_BEGIN(name) struct name : inst {
#define DEFINE_INST_END(name)                                                  \
    static bool classof(const inst *i) { return i->kind == inst::name; }       \
    }

#define DEFINE_INST_FIELD1(t1, n1) t1 n1;
#define DEFINE_INST_FIELD2(t1, n1, t2, n2)                                     \
    t1 n1;                                                                     \
    t2 n2;

#define DEFINE_INST_CTOR0(name)                                                \
    name() : inst(inst::name) {}
#define DEFINE_INST_CTOR1(name, t1, n1)                                        \
    name(const t1 &_##n1) : inst(inst::name), n1(_##n1) {}
#define DEFINE_INST_CTOR2(name, t1, n1, t2, n2)                                \
    name(const t1 &_##n1, const t2 &_##n2)                                     \
        : inst(inst::name), n1(_##n1), n2(_##n2) {}

#define DEFINE_INST_SER_BEGIN                                                  \
    void print_stream(std::ostream &_o) const override {                       \
        _o << get_inst_name();
#define DEFINE_INST_SER_FIELD1(n1) _o << " " << this->n1
#define DEFINE_INST_SER_FIELD2(n1, n2)                                         \
    DEFINE_INST_SER_FIELD1(n1) << ", " << this->n2
#define DEFINE_INST_SER_END }

#define DEFINE_INST_SER0()                                                     \
    DEFINE_INST_SER_BEGIN                                                      \
    DEFINE_INST_SER_END

#define DEFINE_INST_SER1(n1)                                                   \
    DEFINE_INST_SER_BEGIN                                                      \
    DEFINE_INST_SER_FIELD1(n1);                                                \
    DEFINE_INST_SER_END

#define DEFINE_INST_SER2(n1, n2)                                               \
    DEFINE_INST_SER_BEGIN                                                      \
    DEFINE_INST_SER_FIELD2(n1, n2);                                            \
    DEFINE_INST_SER_END

#define DEFINE_INST0(name)                                                     \
    DEFINE_INST_BEGIN(name)                                                    \
    DEFINE_INST_CTOR0(name)                                                    \
    DEFINE_INST_SER0()                                                         \
    DEFINE_INST_END(name)

#define DEFINE_INST1(name, t1, n1)                                             \
    DEFINE_INST_BEGIN(name)                                                    \
    DEFINE_INST_FIELD1(t1, n1)                                                 \
    DEFINE_INST_CTOR1(name, t1, n1)                                            \
    DEFINE_INST_SER1(n1)                                                       \
    DEFINE_INST_END(name)

#define DEFINE_INST2(name, t1, n1, t2, n2)                                     \
    DEFINE_INST_BEGIN(name)                                                    \
    DEFINE_INST_FIELD2(t1, n1, t2, n2)                                         \
    DEFINE_INST_CTOR2(name, t1, n1, t2, n2)                                    \
    DEFINE_INST_SER2(n1, n2)                                                   \
    DEFINE_INST_END(name)

#define GET_DEFINE_INST__(_name, _1, _2, _3, _4, _5, real, ...) real
#define GET_DEFINE_INST(tuple) GET_DEFINE_INST__ tuple

#define DEFINE_INST(...)                                                       \
    GET_DEFINE_INST((__VA_ARGS__, DEFINE_INST2, DEFINE_INST2, DEFINE_INST1,    \
                     DEFINE_INST1, DEFINE_INST0, DEFINE_INST0))                \
    (__VA_ARGS__)

namespace prolog0 {

#define INST_LIST(T)                                                           \
    T(get_structure, functor_t, f, reg_t, x)                                   \
    T(put_structure, functor_t, f, reg_t, x)                                   \
    T(put_value, reg_t, x, reg_t, a)                                           \
    T(get_value, reg_t, x, reg_t, a)                                           \
    T(set_value, reg_t, x)                                                     \
    T(unify_value, reg_t, x)                                                   \
    T(put_variable, reg_t, x, reg_t, a)                                        \
    T(get_variable, reg_t, x, reg_t, a)                                        \
    T(unify_variable, reg_t, x)                                                \
    T(set_variable, reg_t, x)                                                  \
    T(call, functor_t, p)                                                      \
    T(proceed)                                                                 \
    T(allocate, size_t, env_size)                                              \
    T(deallocate)

using reg_t = int32_t;
using addr_t = uintptr_t;
using functor_t = functor;

class inst {

  protected:
    const char *get_inst_name() const;

  public:
    enum kind {
#define T(e, ...) e,
        INST_LIST(T)
#undef T
            NUM_INST
    } kind;

    inst(enum kind kind) : kind(kind) {}

    friend std::ostream &operator<<(std::ostream &, const inst &);
    virtual void print_stream(std::ostream &o) const = 0;

    virtual ~inst() {}
};

#define T(...) DEFINE_INST(__VA_ARGS__);
INST_LIST(T)
#undef T

} // namespace prolog0
