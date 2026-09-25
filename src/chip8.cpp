#include "chip8.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <iostream>

chip8::chip8() : keyboard{}, memory{}, V{}, stack{}, display{} {
  std::ranges::copy(FONTSET, memory.begin() + BEGIN_FONTSET);
}

void chip8::loadROM(const std::string &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    throw std::ios_base::failure("failure to open rom");
  }
  auto size = std::filesystem::file_size(path);
  std::streamsize ssize = static_cast<std::streamsize>(size);

  if (size > MEMORY_SIZE - PROGRAM_START) {
    throw std::ios_base::failure("invalid rom size");
  }

  file.read(reinterpret_cast<char *>(memory.data() + PROGRAM_START), ssize);

  for (size_t i = PROGRAM_START; i < size + PROGRAM_START; i++) {
    std::cout << std::format("{:02X}", memory[i]) << "\n";
  }
}
