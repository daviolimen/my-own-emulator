//
// Created by davi on 2/4/26.
//

#include "computer.h"

// Initialize CPU registers to default values
void initCpu(CPU* cpu) {
    for (uint8_t i = 0; i < 8; i++) cpu->registers[i] = 0;
    cpu->pc = 0;
    cpu->sp = 0x0FFF;
    cpu->flags = 0;
}

// Execute instruction passed as parameter through a function pointer array
// returns true if the instruction is invalid, otherwise false
int executeInstruction(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t opcode = (instruction >> 8) & 0x1F;
    if (opcode > 22) return -1;
    if (opcode == 22) return 0;
    (*operations[opcode])(cpu, memory, instruction);
    return 0;
}

// Executes load lower immediate operation
void exe_lli(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t imm = instruction & 0xFF;
    const uint8_t reg = instruction >> 13;
    cpu->registers[reg] &= 0xFF00;
    cpu->registers[reg] |= imm;
}

// Executes load upper immediate operation
void exe_lui(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t imm = instruction & 0xFF;
    const uint8_t reg = instruction >> 13;
    cpu->registers[reg] &= 0xFF;
    cpu->registers[reg] |= (imm << 8);
}

// Executes MOV operation
void exe_mov(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    instruction &= 0xFF;
    const uint8_t mx = instruction >> 7;
    instruction &= 0x7F;
    const uint8_t rx = instruction >> 4;
    instruction &= 0x0F;
    const uint8_t my = instruction >> 3;
    instruction &= 0x07;
    const uint8_t ry = instruction;
    if (mx) {
        if (my) return; // invalid operation, moving from one address to another directly is not allowed
        memory[cpu->registers[rx]] = cpu->registers[ry];
        memory[cpu->registers[rx] + 1] = cpu->registers[ry] >> 8;
    } else {
        if (my) {
            cpu->registers[rx] = memory[cpu->registers[ry] + 1] << 8 | memory[cpu->registers[ry]];
        } else cpu->registers[rx] = cpu->registers[ry];
    }
}

// Executes NOT operation
void exe_not(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    cpu->registers[rx] = ~cpu->registers[ry];
}

// Executes ADD operation
void exe_add(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] + cpu->registers[rz];
}

// Executes SUB operation
void exe_sub(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] - cpu->registers[rz];
}

// Executes AND operation
void exe_and(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] & cpu->registers[rz];
}

// Executes OR operation
void exe_or(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] | cpu->registers[rz];
}

// Executes XOR operation
void exe_xor(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x07;
    const uint8_t rz = instruction;
    cpu->registers[rx] = cpu->registers[ry] ^ cpu->registers[rz];
}

// Executes left shift operation
void exe_shl(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x0F;
    const uint8_t imm = instruction;
    cpu->registers[rx] = cpu->registers[ry] << imm;
}

// Executes right shift operation
void exe_shr(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    instruction &= 0x0F;
    const uint8_t imm = instruction;
    cpu->registers[rx] = cpu->registers[ry] >> imm;
}

// Executes CMP operation
void exe_cmp(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    instruction &= 0x7F;
    const uint8_t ry = instruction >> 4;
    const int16_t result = (int16_t) (cpu->registers[rx] - cpu->registers[ry]);
    cpu->flags &= ~0x07;
    if (result < 0) cpu->flags |= NFLAG;
    else if (result == 0) cpu->flags |= ZFLAG;
    else cpu->flags |= PFLAG;
}

// Executes JMP operation
void exe_jmp(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    cpu->pc = cpu->registers[rx] - 1;
}

// Executes JEQ operation
void exe_jeq(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (cpu->flags & ZFLAG) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JNE operation
void exe_jne(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (!(cpu->flags & ZFLAG)) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JGT operation
void exe_jgt(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (cpu->flags & PFLAG) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JGE operation
void exe_jge(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (!(cpu->flags & NFLAG)) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JLT operation
void exe_jlt(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (cpu->flags & NFLAG) cpu->pc = cpu->registers[rx] - 1;
}

// Executes JLE operation
void exe_jle(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    if (!(cpu->flags & PFLAG)) cpu->pc = cpu->registers[rx] - 1;
}

// Executes PUSH operation
void exe_push(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    memory[cpu->sp - 1] = cpu->registers[rx] & 0xFF;
    memory[cpu->sp] = cpu->registers[rx] >> 8;
    cpu->sp -= 2;
}

// Executes POP operation
void exe_pop(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    const uint8_t rx = instruction >> 13;
    cpu->sp += 2;
    cpu->registers[rx] = memory[cpu->sp] << 8 | memory[cpu->sp - 1];
}

// Executes HALT operation
void exe_hlt(CPU* cpu, uint8_t* memory, uint16_t instruction) {
    cpu->flags |= HFLAG;
}
