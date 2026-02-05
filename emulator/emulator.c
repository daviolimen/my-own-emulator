//
// Created by davi on 2/4/26.
//

#include "emulator.h"

#include <stdlib.h>

// Function to load the instructions from file to rom array
int loadRom(const char* romPath) {
    FILE* romFile = fopen(romPath, "rb");
    if (romFile == NULL) {
        perror("Error opening rom file");
        return -1;
    }

    fseek(romFile, 0, SEEK_END);
    const long fileSize = ftell(romFile);
    rewind(romFile);

    if (fileSize == -1) {
        perror("Error getting file size");
        fclose(romFile);
        return -1;
    }

    size_t bytesRead = fread(rom, sizeof(uint16_t), fileSize / 2, romFile);
    if (bytesRead != fileSize / 2) {
        perror("Error reading file");
        fclose(romFile);
        return -1;
    }

    fclose(romFile);
    return 0;
}

// Function to get and run the instructions
int runEmulator(const char* romPath) {
    if (loadRom(romPath)) {
        perror("Error loading rom");
        return -1;
    }

    initCpu(&cpu);

    while (true) {
        // Print debug to see the CPU state before each instruction

        for (uint8_t i = 0; i < 8; i++) printf("R%d: %x\n", i, cpu.registers[i]);
        printf("PC: %x\n", cpu.pc);
        printf("SP: %x\n", cpu.sp);
        printf("FLAGS: %x\n", cpu.flags);

        if (cpu.flags & HFLAG) break;
        uint16_t curInstruction = rom[cpu.pc];
        if (executeInstruction(&cpu, memory, curInstruction)) {
            perror("Error executing instruction");
            return -1;
        }
        cpu.pc++;
    }

    return 0;
}