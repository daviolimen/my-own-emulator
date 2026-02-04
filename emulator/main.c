#include "emulator.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        puts("Usage: ./my_own_emulator [ROM path]");
        return -1;
    }

    if (runEmulator(argv[1])) {
        perror("Runtime Error");
        return -1;
    }

    return 0;
}