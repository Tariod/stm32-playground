# stm32-playground

A collection of small projects for learning embedded programming on the **STM32F407G-DISC1** Discovery board, using the
STM32Cube ecosystem.

## Hardware

**Board:** [STM32F407G-DISC1](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) (STM32F4DISCOVERY)

| Feature         | Details                                                                   |
|-----------------|---------------------------------------------------------------------------|
| MCU             | STM32F407VGT6, Arm Cortex-M4 with FPU, up to 168 MHz                      |
| Memory          | 1 MB Flash, 192 KB RAM (incl. 64 KB CCM)                                  |
| Debugger        | On-board ST-LINK/V2-A (SWD), via mini-USB                                 |
| Clock           | 8 MHz external crystal (HSE)                                              |
| User LEDs       | LD4 green (PD12), LD3 orange (PD13), LD5 red (PD14), LD6 blue (PD15)      |
| User button     | B1 (PA0)                                                                  |
| Sensors / audio | LIS3DSH 3-axis accelerometer, MP45DT02 MEMS microphone, CS43L22 audio DAC |
| USB             | USB OTG FS (micro-AB connector)                                           |

Useful references:

- [UM1472 – Discovery kit user manual](https://www.st.com/resource/en/user_manual/um1472-discovery-kit-with-stm32f407vg-mcu-stmicroelectronics.pdf)
- [STM32F407VG datasheet](https://www.st.com/resource/en/datasheet/stm32f407vg.pdf)
- [RM0090 – STM32F4 reference manual](https://www.st.com/resource/en/reference_manual/rm0090-stm32f405415-stm32f407417-stm32f427437-and-stm32f429439-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)

## Toolchain

- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) – pin, clock and peripheral configuration;
  code generation (CMake output)
- [CLion](https://www.jetbrains.com/clion/) – editing, building and debugging
  ([STM32CubeMX projects tutorial](https://www.jetbrains.com/help/clion/embedded-stm32.html))
- [STM32CubeCLT](https://www.st.com/en/development-tools/stm32cubeclt.html) – command-line toolset used by CLion: GNU
  tools for STM32 (`arm-none-eabi-gcc`), CMake, Ninja, ST-LINK GDB server, STM32CubeProgrammer CLI and SVD files
- [STM32CubeF4](https://www.st.com/en/embedded-software/stm32cubef4.html) – HAL/LL drivers, CMSIS and middleware for the
  F4 family

## Repository layout

Each mini-project lives in its own directory and is a self-contained
STM32CubeMX project (with its `.ioc` file) generated with CMake as the
toolchain, so it can be opened directly in CLion.

```
stm32-playground/
├── README.md
├── 01-<project-name>/
├── 02-<project-name>/
├── 03-<project-name>/
└── 04-<project-name>/
```

A generated project looks like this:

```
01-<project-name>/
├── <project-name>.ioc          # STM32CubeMX configuration
├── CMakeLists.txt              # top-level build (user sections can be edited)
├── CMakePresets.json           # Debug / Release presets (Ninja, output in build/)
├── cmake/
│   ├── gcc-arm-none-eabi.cmake # toolchain file
│   └── stm32cubemx/            # CubeMX-managed sources and flags
├── Core/
│   ├── Inc/                    # main.h, HAL config, interrupt headers
│   └── Src/                    # main.c, interrupt handlers, system init
├── Drivers/                    # CMSIS and STM32F4 HAL (copied by CubeMX)
├── Middlewares/                # optional, e.g. USB Host library
├── startup_stm32f407xx.s       # vector table and reset handler
└── STM32F407xx_FLASH.ld        # linker script
```

## Getting started

1. Configure the project in STM32CubeMX and generate code (Project Manager → Toolchain / IDE: **CMake**).
2. Open the project directory (or its `.ioc` file) in CLion. CLion picks up the
   STM32CubeCLT tools and sets up the CMake and debug profiles.
3. Write application code only between `/* USER CODE BEGIN ... */` and
   `/* USER CODE END ... */` comments, so it survives code regeneration.
4. Connect the board via the mini-USB (ST-LINK) port, then build, flash and
   debug from CLion through the ST-LINK GDB server.

## Mini-projects

### 1. TBD

_Description coming soon._

### 2. TBD

_Description coming soon._

### 3. TBD

_Description coming soon._

### 4. TBD

_Description coming soon._
