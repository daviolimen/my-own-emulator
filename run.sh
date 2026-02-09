#!/bin/bash

PROJECT_PATH="/home/davi/CLionProjects/my-own-emulator"
EMULATOR="${PROJECT_PATH}/cmake-build-debug/emulator/emulator"
ASSEMBLER="${PROJECT_PATH}/cmake-build-debug/assembler/assembler"
COMPILER="${PROJECT_PATH}/cmake-build-debug/compiler/compiler"

if [[ $# -lt 2 ]]; then
  printf "Usage: run.sh [option] file\n"
  printf "Options:\n"
  printf "\t-a\t\tAssemble the file passed\n"
  printf "\t-e\t\tEmulate the file passed\n"
  printf "\t-r\t\tAssemble and emulate the file passed\n"
  exit 1
fi

filename=$2

if [[ "$1" == "-c" ]]; then
  ${COMPILER} "$2"
fi

if [[ "$1" == "-a" ]]; then
  ${ASSEMBLER} "$2" "${filename%.*}.bin"
fi

if [[ "$1" == "-e" ]]; then
  ${EMULATOR} "$2"
fi

if [[ "$1" == "-r" ]]; then
  ${ASSEMBLER} "$2" "${filename%.*}.bin"
  ${EMULATOR} "${filename%.*}.bin"
fi