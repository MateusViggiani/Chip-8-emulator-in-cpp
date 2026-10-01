
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <sys/types.h>

class chip8 {
public:
  static constexpr size_t WIDTH = 0x40;
  static constexpr size_t HEIGHT = 0x20;
  uint8_t delayTimer = 0x0;
  uint8_t soundTimer = 0x0;
  chip8();
  void cycle(int button, int released);
  void loadROM(const std::string &caminho);
  const std::array<uint8_t, chip8::WIDTH * chip8::HEIGHT> &giveDisplay() const;
  uint8_t showHEIGHT();
  uint8_t showWIDTH();

private:
  static constexpr size_t REGISTER_SIZE = 0x10;
  static constexpr size_t FONTSET_SIZE = 0x50;
  static constexpr size_t STACK_SIZE = 0x10;
  static constexpr size_t MEMORY_SIZE = 0x1000;
  static constexpr uint16_t PROGRAM_START = 0x200;
  static constexpr uint16_t BEGIN_FONTSET = 0x050;
  static constexpr size_t KEYBOARD_SIZE = 0x10;
  std::array<bool, KEYBOARD_SIZE> keyboard;
  std::array<uint8_t, MEMORY_SIZE> memory;
  std::array<uint8_t, REGISTER_SIZE> V;
  uint16_t pc = PROGRAM_START;
  std::array<uint16_t, STACK_SIZE> stack;
  uint16_t I = 0x0;
  uint8_t sp = 0x0;
  std::array<uint8_t, chip8::WIDTH * chip8::HEIGHT> display;

  void Op00E0();
  void Op00EE();
  void Op1NNN(const uint16_t &NNN);
  void Op2NNN(const uint16_t &NNN);
  void Op3XKK(const uint8_t &X, const uint8_t &KK);
  void Op4XKK(const uint8_t &X, const uint8_t &KK);
  void Op5XY0(const uint8_t &X, const uint8_t &Y);
  void Op6XKK(const uint8_t &X, const uint8_t &KK);
  void Op7XKK(const uint8_t &X, const uint8_t &KK);
  void Op8XY0(const uint8_t &X, const uint8_t &Y);
  void Op8XY1(const uint8_t &X, const uint8_t &Y);
  void Op8XY2(const uint8_t &X, const uint8_t &Y);
  void Op8XY3(const uint8_t &X, const uint8_t &Y);
  void Op8XY4(const uint8_t &X, const uint8_t &Y);
  void Op8XY5(const uint8_t &X, const uint8_t &Y);
  void Op8XY6(const uint8_t &X, const uint8_t &Y);
  void Op8XY7(const uint8_t &X, const uint8_t &Y);
  void Op8XYE(const uint8_t &X, const uint8_t &Y);
  void Op9XY0(const uint8_t &X, const uint8_t &Y);
  void OpANNN(const uint16_t &NNN);
  void OpBNNN(const uint16_t &NNN);
  void OpCXKK(const uint8_t &X, const uint8_t &KK);
  void OpDXYN(const uint8_t &X, const uint8_t &Y, const uint8_t &N);
  void OpEX9E(const uint8_t &X, const int &button);
  void OpEXA1(const uint8_t &X, const int &button);
  void OpFX07(const uint8_t &X);
  void OpFX0A(const uint8_t &X, const int &button);
  void OpFX15(const uint8_t &X);
  void OpFX18(const uint8_t &X);
  void OpFX1E(const uint8_t &X);
  void OpFX29(const uint8_t &X);
  void OpFX33(const uint8_t &X);
  void OpFX55(const uint8_t &X);
  void OpFX65(const uint8_t &X);

  static constexpr std::array<uint8_t, FONTSET_SIZE> FONTSET = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
};
