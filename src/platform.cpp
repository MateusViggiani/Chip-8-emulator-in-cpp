#include "platform.hpp"
#include "chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

platform::platform() : emulator{} {

  if (!initialize()) {
    throw std::runtime_error("Failed to initialize SDL");
  }
}

void platform ::cleanup() {
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
}

bool platform::initialize() {

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Failed to initialize SDL", nullptr);
    cleanup();
    return false;
  }

  state.window = SDL_CreateWindow("Chip8", 1000, 1000, SDL_WINDOW_RESIZABLE);

  if (!state.window) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Failed to open window", nullptr);
    cleanup();
    return false;
  }

  state.renderer = SDL_CreateRenderer(state.window, nullptr);

  if (!state.renderer) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Failed to open renderer", nullptr);
    cleanup();
    return false;
  }

  SDL_SetRenderLogicalPresentation(state.renderer, emulator.WIDTH,
                                   emulator.HEIGHT,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  return true;
}

void platform::run(const std::string &rom) {
  emulator.loadROM(rom);

  for (int k = 0; k < 5000; k++) {
    emulator.cycle();
  }

  std::array<uint32_t, 2048> pixels{};
  const auto &display = emulator.giveDisplay();
  for (size_t i = 0; i < emulator.WIDTH * emulator.HEIGHT; i++) {
    if (display.at(i) == 0) {
      pixels.at(i) = 0xFF000000;
    } else {
      pixels.at(i) = 0XFFFFFFFF;
    }
  }

  SDL_Event event{0};
  SDL_Texture *texture = SDL_CreateTexture(
      state.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
      emulator.WIDTH, emulator.HEIGHT);
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
  bool running = true;
  SDL_UpdateTexture(texture, nullptr, pixels.data(), emulator.WIDTH * 4);
  while (running) {

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
        break;
      }
    }
    SDL_RenderClear(state.renderer);
    SDL_RenderTexture(state.renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(state.renderer);
  }
  SDL_DestroyTexture(texture);
  cleanup();
}
