#include "machine.h"
#include "inst.h"

#define REF 1
#define STR 0

#define GET_TAG(t) ((t) & 1)

namespace {
    uintptr_t pc;

    uintptr_t read(uint8_t *ptr) {
    }
}

namespace prolog0 {

    void interp_get_structure();
    void interp_put_structure();
    void interp_put_variable();
    void interp_put_value();
    void interp_put_variable();
    void interp_get_value();
    void interp_set_value();
    void interp_unify_value();
    void interp_unify_variable();
    void interp_get_variable();
    void interp_set_variable();
    void interp_call();
    void interp_proceed();

    void interp() {
        using interp_t = void (*)();
        static interp_t lut[inst::NUM_INST] = {
#define T(name) interp_##name,
                INST_LIST(T)
#undef T
        };

    }


}
