//
// Created by davi on 2/4/26.
//

#ifndef EMULATOR_EMULATOR_H
#define EMULATOR_EMULATOR_H

#include <stdio.h>

#include "computer.h"

static CPU cpu;
static uint8_t memory[MEM_SIZE];
static uint16_t rom[MEM_SIZE];

int runEmulator(const char* romPath);

#endif //EMULATOR_EMULATOR_H