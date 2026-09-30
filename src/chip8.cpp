#include "chip8.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <random>
#include <sys/types.h>

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

void chip8::Op00EE() {
  sp -= 1;
  pc = stack[sp];
}
void chip8::Op1NNN(const uint16_t &NNN) { pc = NNN; }

void chip8::Op2NNN(const uint16_t &NNN) {
  stack[sp] = pc;
  sp += 1;
  pc = NNN;
}

void chip8::Op3XKK(const uint8_t &X, const uint8_t &KK) {
  if (V[X] == KK) {
    pc += 2;
  }
}

void chip8::Op4XKK(const uint8_t &X, const uint8_t &KK) {
  if (V[X] != KK) {
    pc += 2;
  }
}

void chip8::Op5XY0(const uint8_t &X, const uint8_t &Y) {
  if (V[X] == V[Y])
    pc += 2;
}

void chip8::Op6XKK(const uint8_t &X, const uint8_t &KK) { V[X] = KK; }

void chip8::Op7XKK(const uint8_t &X, const uint8_t &KK) { V[X] += KK; }

void chip8::Op8XY0(const uint8_t &X, const uint8_t &Y) { V[X] = V[Y]; }

void chip8::Op8XY1(const uint8_t &X, const uint8_t &Y) {
  V[X] |= V[Y];
  V[0xF] = 0;
}

void chip8::Op8XY2(const uint8_t &X, const uint8_t &Y) {
  V[X] &= V[Y];
  V[0xF] = 0;
}

void chip8::Op8XY3(const uint8_t &X, const uint8_t &Y) {
  V[X] ^= V[Y];
  V[0xF] = 0;
}

void chip8::Op8XY4(const uint8_t &X, const uint8_t &Y) {
  int result = V[X] + V[Y];

  V[X] += V[Y];
  if (result > 255)
    V[0xF] = 1;
  else
    V[0xF] = 0;
}

void chip8::Op8XY5(const uint8_t &X, const uint8_t &Y) {
  bool carry = V[X] >= V[Y];
  V[X] -= V[Y];
  if (carry)
    V[0xF] = 1;
  else
    V[0xF] = 0;
}

void chip8::Op8XY6(const uint8_t &X, const uint8_t &Y) {
  uint8_t carry = 0x01 & V[Y];
  V[X] = V[Y] >> 1;
  V[0xF] = carry;
}

void chip8::Op8XY7(const uint8_t &X, const uint8_t &Y) {
  bool carry = V[X] <= V[Y];
  V[X] = V[Y] - V[X];
  if (carry)
    V[0xF] = 1;
  else
    V[0xF] = 0;
}

void chip8::Op8XYE(const uint8_t &X, const uint8_t &Y) {
  uint8_t carry = (0x80 & V[Y]) >> 7;
  V[X] = static_cast<uint8_t>(V[Y] << 1);
  V[0xF] = carry;
}

void chip8::Op9XY0(const uint8_t &X, const uint8_t &Y) {
  if (V[X] != V[Y])
    pc += 2;
}

void chip8::OpANNN(const uint16_t &NNN) { I = NNN; }

void chip8::OpBNNN(const uint16_t &NNN) { pc = NNN + V[0]; }

void chip8::OpCXKK(const uint8_t &X, const uint8_t &KK) {

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dist(0, 255);
  V[X] = static_cast<uint8_t>(dist(gen)) & KK;
}

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

void chip8::OpFX07(const uint8_t &X) { V[X] = delayTimer; }

void chip8::OpFX15(const uint8_t &X) { delayTimer = V[X]; }

void chip8::OpFX18(const uint8_t &X) { soundTimer = V[X]; }

void chip8::OpFX1E(const uint8_t &X) { I += V[X]; }

void chip8::OpFX29(const uint8_t &X) {
  I = static_cast<uint16_t>(0x050 + (V[X] & 0xF) * 5);
}

void chip8::OpFX33(const uint8_t &X) {

  uint8_t unidade = static_cast<uint8_t>(V[X] % 10);

  uint8_t dezena = static_cast<uint8_t>(((V[X] - unidade) % 100) / 10);

  uint8_t centena = static_cast<uint8_t>(((V[X] - dezena - unidade)) / 100);

  memory[I] = centena;

  memory[I + 1] = dezena;

  memory[I + 2] = unidade;
}

void chip8::OpFX55(const uint8_t &X) {

  for (uint8_t i = 0; i <= X; i++) {
    memory[I + i] = V[i];
  }
}

void chip8::OpFX65(const uint8_t &X) {
  for (uint8_t i = 0; i <= X; i++) {
    V[i] = memory[I + i];
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
    switch (KK) {
    case 0xE0: { // CLS
      Op00E0();
      break;
    }
    case 0xEE: {
      Op00EE();
      break;
    }
    default: {
      break;
    }
    }
    break;
  }
  case 0x1: { // JP addr
    Op1NNN(NNN);
    break;
  }

  case 0x2: {
    Op2NNN(NNN);
    break;
  }

  case 0x3: {
    Op3XKK(X, KK);
    break;
  }

  case 0x4: {
    Op4XKK(X, KK);
    break;
  }

  case 0x5: {
    Op5XY0(X, Y);
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

  case 0x8: {
    switch (N) {
    case 0x0: {
      Op8XY0(X, Y);
      break;
    }

    case 0x1: {
      Op8XY1(X, Y);
      break;
    }

    case 0x2: {
      Op8XY2(X, Y);
      break;
    }

    case 0x3: {
      Op8XY3(X, Y);
      break;
    }

    case 0x4: {
      Op8XY4(X, Y);
      break;
    }

    case 0x5: {
      Op8XY5(X, Y);
      break;
    }

    case 0x6: {
      Op8XY6(X, Y);
      break;
    }

    case 0x7: {
      Op8XY7(X, Y);
      break;
    }

    case 0xE: {
      Op8XYE(X, Y);
      break;
    }
    }
    break;
  }

  case 0x9: {
    Op9XY0(X, Y);
    break;
  }

  case 0xA: { // LD I, addr
    OpANNN(NNN);
    break;
  }

  case 0xB: {
    OpBNNN(NNN);
    break;
  }

  case 0xC: {
    OpCXKK(X, Y);
    break;
  }

  case 0xD: {
    OpDXYN(X, Y, N);
    break;
  }

  case 0xF: {
    switch (KK) {
    case 0x07: {
      OpFX07(X);
      break;
    }
    case 0x15: {
      OpFX15(X);
      break;
    }
    case 0x18: {
      OpFX18(X);
      break;
    }
    case 0x1E: {
      OpFX1E(X);
      break;
    }
    case 0x29: {
      OpFX29(X);
      break;
    }
    case 0x33: {
      OpFX33(X);
      break;
    }
    case 0x55: {
      OpFX55(X);
      break;
    }
    case 0x65: {
      OpFX65(X);
      break;
    }
    }
  }
  }
}
