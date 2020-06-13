#include <iostream>
#include "src/Repl.h"
int main(int argc, char**argv) {
    if (argc != 2) {
        printf("USAGE: ./db_engine $(DB_FILE_NAME)\n");
        exit(EXIT_FAILURE);
    }
    auto repl = new Repl(argv[1]);
    repl->start();
    delete repl;
    return 0;
}
