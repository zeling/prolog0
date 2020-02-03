#include "scanner.h"
#include "token.h"
#include <iostream>
#include <sstream>
#include <string>

#include "ast.h"
#include "inst.h"
#include "parser.h"
#include "wam.h"
#include "casting.h"

#ifdef HAS_READLINE
#include <readline/history.h>
#include <readline/readline.h>
#endif
#include <vector>

static char *line_read = nullptr;

#ifdef HAS_READLINE
void rl_gets() {
    if (line_read) {
        ::free(line_read);
        line_read = nullptr;
    }
    line_read = readline("prolog0> ");

    if (line_read && *line_read)
        add_history(line_read);
}
#else
void rl_gets() {

}
#endif

int main() {
    using namespace prolog0;

    while (true) {
        rl_gets();
        if (line_read) {
            std::istringstream iss(line_read);
            scanner sc(iss);
            parser p(sc);
            try {
                inst_stream s;
                if (sc.peek() == token::type::QMDASH) {
                    /* its a query */
                    auto qry = p.parse_query();
                    compile_query(s, qry.get());
                    for (auto i : s) {
                        std::cout << *i << std::endl;
                    }
                } else {
                    /* its not a query */
                    auto prg = p.parse_program();
                    compile_program(s, prg.get());
                    for (auto i : s) {
                        std::cout << *i << std::endl;
                    }
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
