cmake_minimum_required(VERSION 4.0)
project(emulator C)

set(CMAKE_C_STANDARD 11)

add_executable(emulator main.c
        computer.c
        computer.h
        emulator.c
        emulator.h)
