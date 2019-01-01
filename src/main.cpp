#include <iostream>
#include <string>
#include <sstream>
#include "scanner.h"
#include "token.h"

#include "ast.h"
#include "wam.h"
#include "parser.h"
#include "rtti.h"
#include "inst.h"
#include "codegen.h"
#include "llvm/Support/Casting.h"

#include <readline/readline.h>
#include <readline/history.h>

static char *line_read = nullptr;
void rl_gets() {
    if (line_read) {
        ::free(line_read);
        line_read = nullptr;
    }
    line_read = readline("prolog0> ");

    if (line_read && *line_read)
        add_history(line_read);
}

int main() {
    using namespace prolog0;

    while (true) {
        rl_gets();
        if (line_read) {
            std::istringstream iss(line_read);
            scanner sc(iss);
            parser p(sc);
            codegen cg;
            if (sc.peek() == token::type::QMDASH) {
                /* its a query */
                auto qry = p.parse_query();
                cg.compile_query(qry.get());
                cg.print_to_stream(std::cout);
            } else {
                /* its not a query */
                auto prg = p.parse_program();
                cg.compile_program(prg.get());
                cg.print_to_stream(std::cout);
            }
        } else {
            break;
        }
    }

    return 0;
}
