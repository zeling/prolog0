#pragma once

#include <iostream>

#define INST_LIST(T) \
  T(get_structure)   \
  T(put_structure)   \
  T(set_value)       \
  T(get_value)       \
  T(set_variable)    \
  T(get_variable)    \


class inst {
public:
    enum kind {
#define T(e) e,
        INST_LIST(T)
#undef T
        NUM_INST
    } kind;

    inst(enum kind kind): kind(kind) {}

    friend std::ostream &operator<<(std::ostream &, const inst &);
};

