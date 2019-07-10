#include "../src/parser.h"
#include "catch.hpp"
#include "llvm/Support/Casting.h"
#include <sstream>

using namespace prolog0;

TEST_CASE("parser", "[parser]") {
    std::istringstream is("person(zeling, male, shanghai) ?-  person(X, Y, "
                          "shanghai).\nperson(X):-hello. ?- person(Y).");
    SECTION("should parse") {
        scanner s(is);
        parser p(s);
        auto trm = p.parse_term();
        structure *str = llvm::dyn_cast<structure>(trm.get());
        REQUIRE(str);
        REQUIRE(str->functor.name == "person");
        REQUIRE(str->functor.arity == 3);
        REQUIRE(llvm::dyn_cast<structure>(str->args[0].get())->functor.name ==
                "zeling");
        REQUIRE(llvm::dyn_cast<structure>(str->args[1].get())->functor.name ==
                "male");
        REQUIRE(llvm::dyn_cast<structure>(str->args[2].get())->functor.name ==
                "shanghai");
        auto qry = p.parse_query();
        REQUIRE(qry->terms.size() == 1);
        str = llvm::dyn_cast<structure>(qry->terms[0].get());
        REQUIRE(str);
        REQUIRE(str->functor.name == "person");
        REQUIRE(str->functor.arity == 3);
        REQUIRE(llvm::dyn_cast<variable>(str->args[0].get())->name == "X");
        REQUIRE(llvm::dyn_cast<variable>(str->args[1].get())->name == "Y");
        REQUIRE(llvm::dyn_cast<structure>(str->args[2].get())->functor.name ==
                "shanghai");
        auto prg = p.parse_program();
        rule *r = llvm::dyn_cast<rule>(prg.get());
        REQUIRE(r);
        str = llvm::dyn_cast<structure>(r->head.get());
        REQUIRE(str);
        REQUIRE(str->functor.name == "person");
        REQUIRE(str->functor.arity == 1);
        REQUIRE(llvm::dyn_cast<variable>(str->args[0].get())->name == "X");
        REQUIRE(r->tail.size() == 1);
        structure *cst = llvm::dyn_cast<structure>(r->tail[0].get());
        REQUIRE(cst);
        REQUIRE(cst->functor.name == "hello");
        auto qry2 = p.parse_query();
    }
}
