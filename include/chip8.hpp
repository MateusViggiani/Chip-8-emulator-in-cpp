
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

class chip8 {
public:
  static constexpr size_t WIDTH = 0x40;
  static constexpr size_t HEIGHT = 0x20;
  chip8();
  void cycle();
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
  uint8_t delayTimer = 0x0;
  uint8_t soundTimer = 0x0;
  std::array<uint8_t, chip8::WIDTH * chip8::HEIGHT> display;

  void Op00E0();
  void Op00EE();
  void Op2NNN(const uint16_t &NNN);
  void Op1NNN(const uint16_t &NNN);
  void Op6XKK(const uint8_t &X, const uint8_t KK);
  void Op7XKK(const uint8_t &X, const uint8_t KK);
  void OpANNN(const uint16_t &NNN);
  void OpDXYN(const uint8_t &X, const uint8_t &Y, const uint8_t &N);

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
