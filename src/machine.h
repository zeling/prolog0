#pragma once

template <typename Integral, size_t S = sizeof(Integral)> struct read_integral {
    Integral do_read(uint8_t *pc);
};
#define INTERP_INST(name) void interp_##name()
