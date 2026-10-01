#include "platform.hpp"
#include "chip8.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
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
  SDL_DestroyAudioStream(state.stream);
  SDL_free(state.buff);
  SDL_Quit();
}

bool platform::initialize() {

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                             "Failed to initialize SDL", nullptr);
    cleanup();
    return false;
  }

  SDL_AudioSpec spec{};

  if (!SDL_LoadWAV("assets/beep.wav", &spec, &state.buff, &state.len)) {
    SDL_Log("Falha ao carregar %s: %s", "assets/beep.wav", SDL_GetError());
    return false;
  }
  state.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                           &spec, nullptr, nullptr);
  SDL_ResumeAudioStreamDevice(state.stream);

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
  if (!SDL_SetRenderVSync(state.renderer, 1)) {
    SDL_Log("VSync indisponível: %s", SDL_GetError());
  }

  return true;
}

int platform::indiceCHIP8(SDL_Scancode sc) {
  int button;
  switch (sc) {
  case SDL_SCANCODE_1: {
    button = 0x1;
    break;
  }
  case SDL_SCANCODE_2: {
    button = 0x2;
    break;
  }
  case SDL_SCANCODE_3: {
    button = 0x3;
    break;
  }
  case SDL_SCANCODE_4: {
    button = 0xC;
    break;
  }
  case SDL_SCANCODE_Q: {
    button = 0x4;
    break;
  }
  case SDL_SCANCODE_W: {
    button = 0x5;
    break;
  }
  case SDL_SCANCODE_E: {
    button = 0x6;
    break;
  }
  case SDL_SCANCODE_R: {
    button = 0xD;
    break;
  }
  case SDL_SCANCODE_A: {
    button = 0x7;
    break;
  }
  case SDL_SCANCODE_S: {
    button = 0x8;
    break;
  }
  case SDL_SCANCODE_D: {
    button = 0x9;
    break;
  }
  case SDL_SCANCODE_F: {
    button = 0xE;
    break;
  }
  case SDL_SCANCODE_Z: {
    button = 0xA;
    break;
  }
  case SDL_SCANCODE_X: {
    button = 0x0;
    break;
  }
  case SDL_SCANCODE_C: {
    button = 0xB;
    break;
  }
  case SDL_SCANCODE_V: {
    button = 0xF;
    break;
  }
  default: {
    button = -1;
    break;
  }
  }
  return button;
}

void platform::run(const std::string &rom) {

  emulator.loadROM(rom);
  std::array<uint32_t, 2048> pixels{};
  const auto &display = emulator.giveDisplay();
  SDL_Event event{0};
  SDL_Texture *texture = SDL_CreateTexture(
      state.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
      emulator.WIDTH, emulator.HEIGHT);
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

  bool running = true;

  const double stepTime = 1.0 / 60.0;

  uint64_t oldTime = SDL_GetTicks();

  uint64_t newTime = 0.0;

  double deltaTime = 0.0;

  double timeAcumulator = 0.0;
  double maxAcumulado = 0.25;
  int button = -1;
  int released = -1;
  while (running) {
    newTime = SDL_GetTicks();
    deltaTime = static_cast<double>((newTime - oldTime)) / 1000.0;
    oldTime = newTime;

    timeAcumulator += deltaTime;
    int k = 0;
    while (SDL_PollEvent(&event)) {

      switch (event.type) {
      case SDL_EVENT_QUIT:
        running = false;
        break;
      case SDL_EVENT_KEY_DOWN: {
        if (event.key.repeat)
          break;
        k = indiceCHIP8(event.key.scancode);
        if (k != -1)
          button = k;
        break;
      }
      case SDL_EVENT_KEY_UP: {
        k = indiceCHIP8(event.key.scancode);
        if (k != -1)
          released = k;
        if (k == button)
          button = -1;
        break;
      }
      }
    }
    if (timeAcumulator > maxAcumulado) {
      timeAcumulator = maxAcumulado;
    }
    while (timeAcumulator >= stepTime) {
      for (size_t i = 0; i < 11; i++) {
        emulator.cycle(button, released);
      }
      released = -1;
      if (emulator.soundTimer > 0) {
        if (static_cast<int>(state.len) >
            SDL_GetAudioStreamQueued(state.stream)) {
          SDL_PutAudioStreamData(state.stream, state.buff,
                                 static_cast<int>(state.len));
        }
        emulator.soundTimer--;
      } else {
        SDL_ClearAudioStream(state.stream);
      }
      if (emulator.delayTimer > 0) {
        emulator.delayTimer--;
      }
      timeAcumulator -= stepTime;
    }
    SDL_RenderClear(state.renderer);
    for (size_t i = 0; i < emulator.WIDTH * emulator.HEIGHT; i++) {
      if (display.at(i) == 0) {
        pixels.at(i) = 0xFF000000;
      } else {
        pixels.at(i) = 0XFFFFFFFF;
      }
    }
    SDL_UpdateTexture(texture, nullptr, pixels.data(), emulator.WIDTH * 4);
    SDL_RenderTexture(state.renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(state.renderer);
  }
  SDL_DestroyTexture(texture);
  cleanup();
}
