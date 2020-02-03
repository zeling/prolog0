#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "inst.h"
#include "machine.h"

namespace {
#define MAX_REGS 256
using wam_addr_t = uintptr_t;

enum tag {
    REF,
    STR = 1,
};

// definition of registers for wam.
wam_addr_t H;
wam_addr_t S;

wam_addr_t regs[MAX_REGS];

constexpr bool is_ref(wam_addr_t addr) { return (addr & STR) == 0; }

constexpr bool is_str(wam_addr_t addr) { return (addr & STR) == STR; }

template <typename Integral, size_t S = sizeof(Integral)> struct read_integral {
    Integral do_read(uint8_t *pc) {
        static_assert(sizeof(Integral) == 1);
        return *pc;
    }
};

#define DECLARE_READ_INTEGRAL(size)                                            \
    template <typename Integral> struct read_integral<Integral, size> {        \
        Integral do_read(uint##size##_t *x) {                                  \
            uint##size##_t r;                                                  \
            memcpy(&r, x, sizeof(r));                                          \
            return r;                                                          \
        }                                                                      \
    }

DECLARE_READ_INTEGRAL(16);
DECLARE_READ_INTEGRAL(32);
DECLARE_READ_INTEGRAL(64);
} // namespace
#define T(name, ...)                                                           \
    void interp_##name() {}
INST_LIST(T)
#undef T

namespace prolog0 {

void interp() {
    using interp_t = void (*)();
    static interp_t lut[inst::NUM_INST] = {
#define T(name, ...) interp_##name,
        INST_LIST(T)
#undef T
    };
}

} // namespace prolog0
