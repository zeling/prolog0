#include <iostream>
#include <string>
#include <sstream>
#include "scanner.h"
#include "token.h"

#include "ast.h"
#include "wam.h"
#include "parser.h"
#include "inst.h"
#include "llvm/Support/Casting.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <vector>

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
//            codegen cg;
            try {
                inst_stream s;
                if (sc.peek() == token::type::QMDASH) {
                    /* its a query */
                    auto qry = p.parse_query();
                    compile_query(s, qry.get());
//                    compile_query_term(s, qry->terms[0].get());
                    for (auto i: s) {
                        std::cout << *i << std::endl;
                    }
//                cg.compile_query_term(qry.get());
//                cg.print_to_stream(std::cout);
                } else {
                    /* its not a query */
                    auto prg = p.parse_program();
                    compile_program(s, prg.get());
                    for (auto i: s) {
                        std::cout << *i << std::endl;
                    }
//                    if (auto f = llvm::dyn_cast<fact>(prg.get())) {
//                        compile_program_term(s, f->_str.get());
//                        for (auto i: s) {
//                            std::cout << *i << std::endl;
//                        }
//                    }
//                    dumb_visitor v;
//                    v.visit(*llvm::dyn_cast<fact>(prg.get()));
//                cg.compile_program(prg.get());
//                cg.print_to_stream(std::cout);
                }
            } catch (const parser_error &e) {
                std::cout << "parse error: " << e.what() << std::endl;
            }
        } else {
            break;
        }
    }

    return 0;
}
