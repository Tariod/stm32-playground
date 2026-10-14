# stm32-playground

A collection of small projects for learning embedded programming on the **STM32F407G-DISC1** Discovery board, using the
STM32Cube ecosystem.

## Hardware

**Board:** [STM32F407G-DISC1](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) (STM32F4DISCOVERY)

| Feature         | Details                                                                                                 |
|-----------------|---------------------------------------------------------------------------------------------------------|
| MCU             | STM32F407VGT6, Arm Cortex-M4 with FPU, up to 168 MHz                                                    |
| Memory          | 1 MB Flash, 192 KB RAM (incl. 64 KB CCM)                                                                |
| Debugger        | On-board ST-LINK/V2-A (SWD), via mini-USB                                                               |
| Clock           | 8 MHz external crystal (HSE)                                                                            |
| User LEDs       | LD4 green (PD12), LD3 orange (PD13), LD5 red (PD14), LD6 blue (PD15)                                    |
| User button     | B1 (PA0)                                                                                                |
| Sensors / audio | LIS3DSH 3-axis accelerometer (LIS302DL on older revisions), MP45DT02 MEMS microphone, CS43L22 audio DAC |
| USB             | USB OTG FS (micro-AB connector)                                                                         |

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

Each mini-project lives in its own directory and is a self-contained STM32CubeMX project (with its `.ioc` file)
generated with CMake as the toolchain, so it can be opened directly in CLion.

```
stm32-playground/
├── .gitignore
├── README.md
├── gated-led-chaser/
├── morse-blinker/
├── spirit-level/
└── code-breaker/
```

A generated project looks like this:

```
<project-name>/
├── <project-name>.ioc          # STM32CubeMX configuration
├── CMakeLists.txt              # top-level build (user sections can be edited)
├── CMakePresets.json           # Debug / Release presets (Ninja, output in build/)
├── cmake/
│   ├── gcc-arm-none-eabi.cmake # toolchain file
│   └── stm32cubemx/            # CubeMX-managed sources and flags
├── Core/
│   ├── Inc/                    # main.h, HAL config, interrupt headers
│   └── Src/                    # main.c, interrupt handlers, system init
├── Drivers/                    # CMSIS and STM32F4 HAL (copied by CubeMX), optional BSP (added manually)
├── Middlewares/                # optional, e.g. USB Host library
├── startup_stm32f407xx.s       # vector table and reset handler
└── STM32F407xx_FLASH.ld        # linker script
```

## Getting started

1. Configure the project in STM32CubeMX and generate code (Project Manager → Toolchain / IDE: **CMake**).
2. Open the project directory (or its `.ioc` file) in CLion. CLion picks up the STM32CubeCLT tools and sets up the CMake
   and debug profiles.
3. Write application code only between `/* USER CODE BEGIN ... */` and `/* USER CODE END ... */` comments, so it
   survives code regeneration.
4. Connect the board via the mini-USB (ST-LINK) port, then build, flash and debug from CLion through the ST-LINK GDB
   server.

## Mini-projects

### 1. gated-led-chaser

A pair of adjacent user LEDs chases around the ring of four LEDs, moving one step per second (orange → green → blue →
red, counter-clockwise on the board). The chase only advances while the `SIGNAL_IN` input is high, so it can be started
and paused with a wire.

**Wiring:** connect `PA1` to the nearest `VDD` pin on the header to run the chaser; disconnect it to pause. `PA1` has an
internal pull-down, so it reads low when left floating.

**Concepts:**

- GPIO outputs (`LD3`–`LD6` on `PD12`–`PD15`) and a GPIO input with a pull-down resistor
- CubeMX pin labels turning into macros such as `SIGNAL_IN_Pin` / `SIGNAL_IN_GPIO_Port`
- Non-blocking timing with `HAL_GetTick()` instead of `HAL_Delay()`, so the main loop keeps running between steps

### 2. morse-blinker

Blinks the message `"Hello World!"` in Morse code on the green LED (`LD4`), once after reset. Letters and digits are
supported; other characters (like `!`) are skipped.

**Timing:** one unit is 200 ms (`MORSE_UNIT_MS`). A dot lasts 1 unit and a dash 3 units, with gaps of 1 unit between
signals, 3 between letters and 7 between words.

**Concepts:**

- Compact lookup table: each Morse code packed into one byte (low 3 bits = length, upper bits = dots/dashes)
- Bit manipulation to decode the signals
- Blocking timing with `HAL_Delay()`, in contrast to the non-blocking approach in `gated-led-chaser`

### 3. spirit-level

Turns the board into a spirit level: tilting it lights the LED on the matching side. Every 100 ms the on-board
accelerometer is read, and an LED turns on when the X or Y reading passes ±20 (red `LD5` / green `LD4` for X, orange
`LD3` / blue `LD6` for Y). When the board lies flat, all LEDs stay off.

**Concepts:**

- Using ST's board support package (BSP) for the Discovery board (`Drivers/BSP/`), added to `CMakeLists.txt` by hand
- Reading the accelerometer with `BSP_ACCELERO_Init()` / `BSP_ACCELERO_GetXYZ()`; the BSP detects whether the board has
  a LIS3DSH or LIS302DL and sets up SPI itself, so SPI1 is not enabled in CubeMX
- Non-blocking periodic polling with `HAL_GetTick()`

### 4. code-breaker

A guessing game played on an external 4×4 matrix keypad. The board picks a secret 4-digit code, and you crack it one
digit at a time:

- each correct digit lights the next LED (orange → green → blue → red);
- a wrong digit blinks all LEDs twice and sends you back to the first digit (the code stays the same);
- after all four digits, the LEDs blink 8 times and a new code is generated.

Only the digit keys are used; `A`–`D`, `*` and `#` are ignored.

**Wiring:** keypad rows to `PD0`–`PD3` (`KEYPAD_ROW_1`–`KEYPAD_ROW_4`, outputs) and columns to `PD8`–`PD11`
(`KEYPAD_COL_1`–`KEYPAD_COL_4`, inputs with internal pull-ups).

**Concepts:**

- Matrix keypad scanning: one row is pulled low every 2 ms and the columns are read, packing all 16 keys into a
  bit-field union (`Key_State`)
- Debouncing (a key must read the same for 3 full scans), press-only reporting, and ignoring multi-key presses to avoid
  ghosting
- Hardware random number generator (`RNG`) with rejection sampling for unbiased digits
- A small non-blocking state machine (playing / error blink / win blink) driven by `HAL_GetTick()`
- Splitting code into a separate module (`keypad.c` / `keypad.h`) added to `CMakeLists.txt`
