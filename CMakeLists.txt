cmake_minimum_required(VERSION 3.25)

project(chip8 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

find_package(SDL3 QUIET)


if(NOT SDL3_FOUND)
  message(STATUS "SDL3 nao encontrada no sistema, baixando ...")
  FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(SDL3)
else()
  message(STATUS "usando SDL3 do sistema")
endif()

add_executable(chip8
  main.cpp
  src/chip8.cpp
  src/platform.cpp
)

target_include_directories(chip8 PRIVATE include)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)

target_compile_options(chip8 PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wimplicit-fallthrough)

add_custom_command(TARGET chip8 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/roms
  $<TARGET_FILE_DIR:chip8>/roms)
