//
// Created by davi on 2/4/26.
//

#include "computer.h"

// Initialize CPU registers to default values
void initCpu(CPU* cpu) {
    for (uint8_t i = 0; i < 7; i++) cpu->registers[i] = 0;
    cpu->registers[7] = 0xFFFF; // Internally, R7 is the stack pointer (SP)
    cpu->pc = 0;
    cpu->flags = 0;
}

// Executes load lower immediate operation
static void exe_lli(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t imm = instruction & 0xFF;
    const uint8_t reg = instruction >> 13;
    cpu->registers[reg] &= 0xFF00;
    cpu->registers[reg] |= imm;
}

// Executes load upper immediate operation
static void exe_lui(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t imm = instruction & 0xFF;
    const uint8_t reg = instruction >> 13;
    cpu->registers[reg] &= 0xFF;
    cpu->registers[reg] |= (imm << 8);
}

// Executes MOV operation
static void exe_mov(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    instruction &= 0xFF;
    const uint8_t mx = instruction >> 7;
    instruction &= 0x7F;
    const uint8_t rx = instruction >> 4;
    instruction &= 0x0F;
    const uint8_t my = instruction >> 3;
    instruction &= 0x07;
    const uint8_t ry = instruction;
    if (mx) {
        if (my) memory[cpu->registers[rx]] = memory[cpu->registers[ry]];
        else memory[cpu->registers[rx]] = cpu->registers[ry];
    } else {
        if (my) cpu->registers[rx] = memory[cpu->registers[ry]];
        else cpu->registers[rx] = cpu->registers[ry];
    }
}

// Executes NOT operation
static void exe_not(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    cpu->registers[rx] = ~cpu->registers[ry];
}

// Executes ADD operation
static void exe_add(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] + cpu->registers[rz];
}

// Executes SUB operation
static void exe_sub(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] - cpu->registers[rz];
}

// Executes AND operation
static void exe_and(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] & cpu->registers[rz];
}

// Executes OR operation
static void exe_or(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] | cpu->registers[rz];
}

// Executes XOR operation
static void exe_xor(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] ^ cpu->registers[rz];
}

// Executes left shift operation
static void exe_shl(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x0F;
    const uint8_t imm = instruction;
    cpu->registers[rx] = cpu->registers[ry] << imm;
}

// Executes right shift operation
static void exe_shr(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x0F;
    const uint8_t imm = instruction;
    cpu->registers[rx] = cpu->registers[ry] >> imm;
}

// Executes CMP operation
static void exe_cmp(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    cpu->flags &= ~0x07;
    if (cpu->registers[rx] > cpu->registers[ry]) cpu->flags |= PFLAG;
    else if (cpu->registers[rx] == cpu->registers[ry]) cpu->flags |= ZFLAG;
    else cpu->flags |= NFLAG;
}

// Executes JMP operation
static void exe_jmp(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    cpu->pc = cpu->registers[rx] - 1;
}

// Executes JEQ operation
static void exe_jeq(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (cpu->flags & ZFLAG) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JNE operation
static void exe_jne(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (!(cpu->flags & ZFLAG)) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JGT operation
static void exe_jgt(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (cpu->flags & PFLAG) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JGE operation
static void exe_jge(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (!(cpu->flags & NFLAG)) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JLT operation
static void exe_jlt(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (cpu->flags & NFLAG) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JLE operation
static void exe_jle(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (!(cpu->flags & PFLAG)) cpu->pc = cpu->registers[rx] - 1;
}

// Executes PUSH operation
static void exe_push(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    memory[cpu->registers[7]] = cpu->registers[rx];
    cpu->registers[7]--;
}

// Executes POP operation
static void exe_pop(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    cpu->registers[7]++;
    cpu->registers[rx] = memory[cpu->registers[7]];
}

// Executes HALT operation
static void exe_hlt(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    cpu->flags |= HFLAG;
}

static void (*operations[])(CPU*, uint16_t*, uint16_t) = {
    exe_lli, exe_lui, exe_mov, exe_not, exe_add, exe_sub, exe_and, exe_or, exe_xor, exe_shl, exe_shr, exe_cmp, exe_jmp,
    exe_jeq, exe_jne, exe_jgt, exe_jge, exe_jlt, exe_jle, exe_push, exe_pop, exe_hlt
};

// Execute instruction passed as parameter through a function pointer array
// returns true if the instruction is invalid, otherwise false
int executeInstruction(CPU* cpu, uint16_t* memory, uint16_t instruction) {
    const uint8_t opcode = (instruction >> 8) & 0x1F;
    if (opcode > 22) return -1;
    if (opcode == 22) return 0;
    (*operations[opcode])(cpu, memory, instruction);
    return 0;
}