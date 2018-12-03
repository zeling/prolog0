#include "../src/codegen.h"
#include "../src/scanner.h"
#include "../src/parser.h"
#include "catch.hpp"
#include <sstream>
#include "llvm/Support/Casting.h"

using namespace prolog0;

TEST_CASE("alloc_reg", "[codegen]") {
    std::istringstream iss("p(Z, h(Z, W), f(W))");

    SECTION("alloc_reg") {
        scanner sc(iss);
        parser p(sc);

        auto term = p.parse_term();
        reg_map_t rmap;
        wam_reg_t cur = 0;

        alloc_reg(term.get(), rmap, cur, true);

        auto str = llvm::dyn_cast<structure>(term.get());
        REQUIRE(rmap[str->args[0].get()] == 4);
        REQUIRE(rmap[str->args[1].get()] == 5);

        auto str_h = llvm::dyn_cast<structure>(str->args[1].get());

        REQUIRE(rmap[str_h->args[0].get()] == 4);
        REQUIRE(rmap[str_h->args[1].get()] == 6);

        REQUIRE(rmap[str->args[2].get()] == 7);

        auto str_f = llvm::dyn_cast<structure>(str->args[2].get());

        REQUIRE(rmap[str_f->args[0].get()] == 6);
    }

}
