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
    uint16_t sp;
    uint16_t flags;
} CPU;

#define NFLAG (1 << 0)
#define ZFLAG (1 << 1)
#define PFLAG (1 << 2)
#define HFLAG (1 << 3)

void exe_lli(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_lui(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_mov(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_not(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_add(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_sub(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_and(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_or(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_xor(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_shl(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_shr(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_cmp(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_jmp(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_jeq(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_jne(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_jgt(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_jge(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_jlt(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_jle(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_push(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_pop(CPU* cpu, uint8_t* memory, uint16_t instruction);
void exe_hlt(CPU* cpu, uint8_t* memory, uint16_t instruction);

static void (*operations[])(CPU*, uint8_t*, uint16_t) = {
    exe_lli, exe_lui, exe_mov, exe_not, exe_add, exe_sub, exe_and, exe_or, exe_xor, exe_shl, exe_shr, exe_cmp, exe_jmp,
    exe_jeq, exe_jne, exe_jgt, exe_jge, exe_jlt, exe_jle, exe_push, exe_pop, exe_hlt
};

void initCpu(CPU* cpu);

int executeInstruction(CPU* cpu, uint8_t* memory, uint16_t instruction);

#endif //EMULATOR_COMPUTER_H