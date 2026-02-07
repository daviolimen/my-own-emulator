find_package(SDL2 REQUIRED)

add_executable(emulator main.c
        computer.c
        computer.h
        emulator.c
        emulator.h
        graphics.c
        graphics.h)

target_link_libraries(emulator SDL2::SDL2)