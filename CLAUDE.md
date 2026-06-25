# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

`teensy-x86-spi-stubs` is a **non-functional stub** of the Teensy 4.x Arduino `SPI` library, compiled for the host (x86) instead of the embedded target. Its sole purpose: let Teensy/Arduino sketches that `#include <SPI.h>` compile and link on a desktop so they can be built and unit-tested off-device, without wrapping every SPI include/call in `#ifdef` compiler directives. The method bodies are intentionally empty — they return defaults (`0`/`false`/`true`) and do no real I/O. Do not "implement" them to do real SPI; matching the upstream Teensy `SPI.h` API surface (so host builds link) is the goal, not behavior.

This is one of a family of `teensy_x86_*_stubs` libraries. It depends on a sibling package `teensy_x86_stubs`, which supplies the Arduino/Teensy headers (`Arduino.h`, `imxrt.h`, register macros like `CCM_CCGR1`, `IMXRT_LPSPI4_ADDRESS`, types like `IRQ_NUMBER_t`). That package is **not present in this repo** and must be installed for builds to find it via `find_package(teensy_x86_stubs)`.

## Build

CMake project, C++14. The root `CMakeLists.txt` defines `ARDUINO_TEENSY41` globally and descends into `src/`, which builds the `teensy_x86_spi_stubs` library and CMake package config/install rules.

```sh
cmake -S . -B build      # requires teensy_x86_stubs installed/findable
cmake --build build
cmake --install build    # installs lib + headers + <pkg>Config.cmake
cmake --build build --target uninstall   # removes installed files via install_manifest.txt
```

There are no tests, linters, or CI in this repo — it is a leaf dependency consumed by other host-build projects.

## Things to know when editing

- **`ARDUINO_TEENSY41` changes struct layout.** `SPI_Hardware_t` (in `src/SPI.h`) sizes its pin arrays from `CNT_*_PINS` constants that differ between T4.1 and other Teensy 4.x. The matching hardware-table initializers in `src/SPI.cpp` are wrapped in `#if defined(ARDUINO_TEENSY41)` and the field count per group changes accordingly — if you touch one branch, keep the other in sync.
- **`constexpr` constructor / static init is load-bearing.** `SPIClass` takes `uintptr_t` port/hardware addresses (not typed pointers) on purpose: the commented-out typed-pointer constructor breaks `constinit` static initialization of the global `SPI`/`SPI1`/`SPI2` instances. See the note and PJRC forum link near the bottom of `src/SPI.cpp` before changing the constructor signature.
- The three global instances `SPI`, `SPI1`, `SPI2` map to LPSPI4/LPSPI3/LPSPI1 respectively.
