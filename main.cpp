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
    return 1;
  }

  for (int k = 0; k < 40; k++) {
    emulator.cycle();
  }
  const auto &display = emulator.giveDisplay();
  for (size_t i = 0; i < emulator.HEIGHT; i++) {
    for (size_t j = 0; j < emulator.WIDTH; j++) {
      auto pos = i * emulator.WIDTH + j;
      try {
        if (display.at(pos) == 1) {
          std::cout << '#';
        } else {
          std::cout << '-';
        }
      } catch (std::exception &e) {
        std::cout << pos;
        std::cout << "\n";
        return 1;
      }
    }
    std::cout << "\n";
  }
  return 0;
}
