#pragma once

#include "chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <string>

struct sdlState {
  SDL_Window *window;
  SDL_Renderer *renderer;
  const int width = 64;
  const int height = 32;
};

class platform {

  sdlState state;
  chip8 emulator;

  bool initialize();
  void cleanup();

public:
  platform();

  void run(const std::string &rom);
};
