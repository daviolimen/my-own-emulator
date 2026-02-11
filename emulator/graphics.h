//
// Created by davi on 2/7/26.
//

#ifndef MY_OWN_EMULATOR_GRAPHICS_H
#define MY_OWN_EMULATOR_GRAPHICS_H

#include <SDL2/SDL.h>

#define FRAMEBUFFER_ADDRESS 0x2000

int initWindow();
int windowLoop(const uint16_t* framebuffer);

#endif //MY_OWN_EMULATOR_GRAPHICS_H