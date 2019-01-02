#include <string>
#include <vector>
#include <unordered_map>

#include "machine.h"
#include "inst.h"
#include "endian.h"

#define REF 1
#define STR 0

#define GET_TAG(t) ((t) & 1)

namespace {
    prolog0::wam_addr_t pc;
    prolog0::wam_addr_t h;
    prolog0::wam_addr_t s;

    std::vector<prolog0::wam_reg_t> regs;

    std::unordered_map<std::string,

    template <typename Integral, size_t S = sizeof(Integral)>
    struct read_integral {
        Integral do_read(uint8_t *pc);
    };

    template <typename Integral>
    struct read_integral<Integral, 16> {
        Integral do_read(uint8_t *pc) {
            return be16toh(*reinterpret_cast<uint16_t *>(pc));
        }
    };
    template <typename Integral>
    struct read_integral<Integral, 32> {
        Integral do_read(uint8_t *pc) {
            return be32toh(*reinterpret_cast<uint32_t *>(pc));
        }
    };
    template <typename Integral>
    struct read_integral<Integral, 64> {
        Integral do_read(uint8_t *pc) {
            return be64toh(*reinterpret_cast<uint64_t *>(pc));
        }
    };

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

}

namespace prolog0 {

    void interp() {
        using interp_t = void (*)();
        static interp_t lut[inst::NUM_INST] = {
#define T(name) interp_##name,
                INST_LIST(T)
#undef T
        };

    }


}
