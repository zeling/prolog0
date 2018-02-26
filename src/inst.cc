#include <iostream>
#include "inst.h"


std::ostream &operator<<(std::ostream &os, const inst &i) {
    static const char *inst_names[] = {
#define T(e) #e,
    INST_LIST(T)
#undef T
    };
    os << inst_names[i.kind];
    return os;
}
