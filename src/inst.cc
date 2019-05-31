#include <iostream>
#include "inst.h"



namespace prolog0 {

    const char *inst::get_inst_name() const {
        static const char *inst_names[] = {
#define T(e, ...) #e,
                INST_LIST(T)
#undef T
        };
        return inst_names[kind];
    }

    std::ostream &operator<<(std::ostream &os, const inst &i) {
        i.print_stream(os);
        return os;
    }
}
