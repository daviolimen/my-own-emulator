//
// Created by davi on 2/4/26.
//

#ifndef EMULATOR_COMPUTER_H
#define EMULATOR_COMPUTER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEM_SIZE (1 << 16)

typedef struct {
    uint16_t registers[8];
    uint16_t pc;
    uint16_t flags;
} CPU;

#define NFLAG (1 << 0)
#define ZFLAG (1 << 1)
#define PFLAG (1 << 2)
#define HFLAG (1 << 3)

void initCpu(CPU* cpu);

int executeInstruction(CPU* cpu, uint8_t* memory, uint16_t instruction);

#endif //EMULATOR_COMPUTER_H