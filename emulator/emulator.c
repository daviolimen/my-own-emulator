//
// Created by davi on 2/4/26.
//

#include "emulator.h"

#include <stdlib.h>
#include <time.h>

#include "graphics.h"

// enable/disable debug mode
// #define DEBUG_MODE

// timing-related defines
#define CPU_FREQ_HZ 1000000LL
#define CPU_PERIOD_NS (1000000000LL / CPU_FREQ_HZ)
#define FRAMES_PER_SECOND 60
#define CYCLES_PER_FRAME (CPU_FREQ_HZ / FRAMES_PER_SECOND)

CPU cpu;
uint16_t memory[MEM_SIZE];
uint16_t rom[MEM_SIZE];

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

    size_t dataRead = fread(memory, sizeof(uint16_t), 0x2000, romFile);
    if (dataRead != 0x2000) {
        perror("Error loading data from rom");
        return -1;
    }

    size_t instructionsRead = fread(rom, sizeof(uint16_t), (fileSize - 0x4000) / 2, romFile);
    if (instructionsRead != ((fileSize - 0x4000) / 2)) {
        perror("Error loading instructions from rom");
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
    initWindow();

#ifdef DEBUG_MODE
    unsigned waitCnt = 0;
#endif

    // I don't know much about timing in C, so I don't know if this is the usual way to handle timing, but anyways...
    long long cyclesPassed = 0;
    struct timespec prevTime, curTime;
    timespec_get(&prevTime, TIME_UTC);
    while (true) {
        timespec_get(&curTime, TIME_UTC);
        long long diff = (curTime.tv_sec - prevTime.tv_sec) * 1000000000LL + (curTime.tv_nsec - prevTime.tv_nsec);
        if (diff < CPU_PERIOD_NS) continue;
        prevTime = curTime;

        if (cyclesPassed >= CYCLES_PER_FRAME) {
            if (windowLoop(memory + FRAMEBUFFER_ADDRESS) == -1) {
                printf("SDL requested quit\n");
                cpu.flags |= HFLAG;
            }
            cyclesPassed = 0;
        }
        cyclesPassed++;

        uint16_t curInstruction = rom[cpu.pc];

        if (executeInstruction(&cpu, memory, curInstruction)) {
            perror("Error executing instruction");
            return -1;
        }


#ifdef DEBUG_MODE
        // Print debug to see the CPU state after each instruction

        fputs("\e[1;1H\e[2J", stdout);
        fflush(stdout);
        puts("EMULATOR DEBUGGER");
        printf("\tCurrent instruction: 0x%x", curInstruction);
        if (cpu.flags & HFLAG) printf(" (HLT, the execution has halted)");
        printf("\n");
        printf("\tPC: 0x%x\n", cpu.pc);
        printf("\tSP: 0x%x\n", cpu.sp);
        printf("\tFLAGS: 0x%x\n", cpu.flags);

        while (true) {
            if (cpu.flags & HFLAG) waitCnt = 0;
            if (waitCnt > 0) {
                waitCnt--;
                break;
            }
            printf("> ");
            char command;
            char str[32];
            scanf(" %c", &command);
            if (command == 'N') break; // go to next instruction
            scanf("%s", str);
            unsigned num = strtol(str, NULL, 0);
            if (command == 'R') { // print register
                if (num < 8) printf("Register %d: 0x%x\n", num, cpu.registers[num]);
                else printf("Invalid register number\n");
            } else if (command == 'M') { // print memory
                if (num < MEM_SIZE) printf("Memory[0x%x]: 0x%x\n", num, memory[num]);
                else printf("Invalid memory address\n");
            } else if (command == 'W') { // wait steps
                waitCnt = num;
            } else puts("Invalid instruction");
        }

#endif // DEBUG_MODE

        if (cpu.flags & HFLAG) break;

        cpu.pc++;
    }

    return 0;
}