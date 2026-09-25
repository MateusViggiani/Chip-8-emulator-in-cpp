#include "chip8.hpp"
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    return 0;
  }

  chip8 emulator{};

  try {
    emulator.loadROM(argv[1]);
  } catch (std::exception &e) {
    std::cerr << e.what();
  }
  return 0;
}
