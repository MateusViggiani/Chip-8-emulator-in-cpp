#include "platform.hpp"
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    return 0;
  }

  std::cout << "teste";
  try {
    platform emulator{};
    emulator.run(argv[1]);
  } catch (const std::exception &e) {
    std::cerr << e.what();
  }

  return 0;
}
