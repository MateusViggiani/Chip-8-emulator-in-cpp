#pragma once

#include "chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <string>

struct sdlState {
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  const int width = 64;
  const int height = 32;
  Uint8 *buff = nullptr;
  SDL_AudioStream *stream;
  Uint32 len = 0;
};

class platform {

  sdlState state;
  chip8 emulator;
  constexpr std::array<SDL_Scancode, 16> static KEYMAP = {
      SDL_SCANCODE_X,                                 // 0x0
      SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, // 0x1 0x2 0x3
      SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, // 0x4 0x5 0x6
      SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, // 0x7 0x8 0x9
      SDL_SCANCODE_Z, SDL_SCANCODE_C,                 // 0xA 0xB
      SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F,
      SDL_SCANCODE_V, // 0xC 0xD 0xE 0xF
  };
  bool initialize();
  void cleanup();
  int indiceCHIP8(SDL_Scancode sc);

public:
  platform();

  void run(const std::string &rom);
};
