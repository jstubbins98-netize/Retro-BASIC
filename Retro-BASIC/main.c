#include "config.h"
#include "io.h"
#include "vars.h"
#include "program.h"
#include "commands.h"
#include "repl.h"

int main(int argc, char *argv[])
{
    vars_init();
    program_init();

    basic_print("\n");
    basic_print("RETRO BASIC V2.0\n");
    basic_print("FREE BYTES: ");
    print_num(MAX_PROG);
    basic_print("\n\n");

    if (argc > 1) {
        if (!program_load(argv[1])) {
            basic_print("?CANNOT OPEN FILE: ");
            basic_print(argv[1]);
            basic_print("\n");
        } else {
            run_program();
        }
    }

    repl();

    return 0;
}
