#include "machine.h"
#include "inst.h"

#define REF 1
#define STR 0

#define GET_TAG(t) ((t) & 1)

namespace {
    uintptr_t pc;

    uintptr_t read(uint8_t *ptr) {
        uintptr_t
    }
}

namespace prolog0 {
    void interp() {
        static void *lut[inst::NUM_INST] = {
#define T(name) interp_##name,
                INST_LIST(T)
#undef T
        };

    }

    void interp_put_structure() {
        wam_functor_t
    }


    void unify()
}
