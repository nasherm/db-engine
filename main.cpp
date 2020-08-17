#include <iostream>
#include "src/Repl.h"
int main(int argc, char**argv) {
    auto repl = (argc == 2) ? Repl(argv[1]) : Repl();
    repl.start();
}
