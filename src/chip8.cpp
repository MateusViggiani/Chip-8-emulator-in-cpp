#include "chip8.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
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
}

void chip8::Op00E0() { display.fill(0); };

void chip8::Op1NNN(const uint16_t &NNN) { pc = NNN; }

void chip8::Op6XKK(const uint8_t &X, const uint8_t KK) { V[X] = KK; }

void chip8::Op7XKK(const uint8_t &X, const uint8_t KK) { V[X] += KK; }

void chip8::OpANNN(const uint16_t &NNN) { I = NNN; }

void chip8::OpDXYN(const uint8_t &X, const uint8_t &Y, const uint8_t &N) {

  uint8_t startCol = V[X] % WIDTH; // posição inicial do pixel display

  uint8_t startRow = V[Y] % HEIGHT;

  V[0xF] = 0; // se deu conflito

  for (unsigned int row = 0; row < N; row++) {
    if (row + startRow >= HEIGHT)
      break;
    uint8_t spriteByte =
        memory[I +
               row]; // pegar os sprites (os bits do byte são tipo uma tinta)

    for (unsigned int column = 0; column < 8; column++) {
      if (column + startCol >= WIDTH)
        break;

      uint8_t bit =
          (spriteByte >> (7 - column)) & 1; // extrai os bits(Tinta da tela)

      uint8_t *screenPixel =
          &display[(startRow + row) * WIDTH +
                   (startCol + column)]; // equivalente a
                                         // display[startRow+row][startCol+col]

      if (bit) {

        if (*screenPixel == 0x1) {
          V[0xF] = 0x1;
        }

        *screenPixel ^= 0x1;
      }
    }
  }
}

const std::array<uint8_t, chip8::HEIGHT * chip8::WIDTH> &
chip8::giveDisplay() const {
  return display;
}

void chip8::cycle() {

  uint16_t opcode = static_cast<uint16_t>(
      (memory[pc + 1]) | (static_cast<uint16_t>(memory[pc]) << 8));
  pc += 2;
  uint8_t X = static_cast<uint8_t>((opcode & 0x0F00) >> 8);
  uint8_t N = static_cast<uint8_t>((opcode & 0x00F));
  uint8_t Y = static_cast<uint8_t>((opcode & 0x00F0) >> 4);
  uint8_t KK = static_cast<uint8_t>(opcode & 0x00FF);
  uint16_t NNN = (opcode & 0x0FFF);
  switch (opcode >> 12) {
  case 0x0: {
    switch (opcode) {
    case 0x00E0: { // CLS
      Op00E0();
      break;
    }
    default: {
      std::cout << std::format("{:04X} ", opcode);
      std::cout << "\n";
      break;
    }
    }
    break;
  }
  case 0x1: { // JP addr
    Op1NNN(NNN);
    break;
  }

  case 0x6: { // LD Vx, byte
    Op6XKK(X, KK);
    break;
  }

  case 0x7: { // ADD Vx,byte
    Op7XKK(X, KK);
    break;
  }

  case 0xA: { // LD I, addr
    OpANNN(NNN);
    break;
  }

  case 0xD: {
    OpDXYN(X, Y, N);
    break;
  }
  }
}
