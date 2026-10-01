# Chip-8-emulator-in-cpp

Emulador de CHIP-8 em C++23 com SDL3.

## Como rodar

Requer CMake 3.25+ e um compilador com suporte a C++23. Se a SDL3 não estiver instalada, o CMake baixa automaticamente.

```sh
cmake -B build
cmake --build build
./build/chip8 roms/pong.rom
```

Rode a partir da raiz do projeto: o som é carregado de `assets/beep.wav`.

## Teclado

```
1 2 3 4      1 2 3 C
Q W E R  ->  4 5 6 D
A S D F      7 8 9 E
Z X C V      A 0 B F
```

## Quirks

- **VF reset**: `8XY1`, `8XY2` e `8XY3` zeram VF.
- **Shift**: `8XY6` e `8XYE` usam VY como origem (comportamento original do COSMAC VIP).
- **Memória**: `FX55` e `FX65` não alteram I.
- **Jump**: `BNNN` salta para `NNN + V0`.
- **Clipping**: sprites são cortados nas bordas; só a posição inicial dá a volta na tela.
- **FX0A**: espera a tecla ser solta.
- **Display wait**: não implementado.
- Velocidade: 11 instruções por frame a 60 Hz; timers decrementam a 60 Hz.
