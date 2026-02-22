# Joystick Interface — CH32X033

A minimal joystick controller firmware for the **CH32X035** (WCH RISC-V) that
feeds directional and button inputs to a **Pac-Man** arcade core running on a
**Gowin FPGA** with a **T80 (Z80-compatible)** CPU.

## How it works

An analog joystick (VRx/VRy/SW) is read via ADC. Movements and button presses
are classified into events and transmitted as single-byte commands over a
software UART (PA10, 9600 8N1) to the FPGA.

## Commands

| Byte | Command | Trigger |
|------|---------|---------|
| `1`  | UP      | Joystick up |
| `2`  | DOWN    | Joystick down |
| `3`  | LEFT    | Joystick left |
| `4`  | RIGHT   | Joystick right |
| `5`  | START   | Single press of SW |
| `6`  | COIN    | Double press of SW within 300 ms |

## Pinout

| Pin  | Signal | Description |
|------|--------|-------------|
| PA1  | VRx    | X-axis ADC input |
| PA2  | VRy    | Y-axis ADC input |
| PA3  | SW     | Push-button (active-low, pull-up) |
| PA10 | TX     | UART command output to FPGA |

## Log output

Debug logs are routed to a USB CDC serial port (enumerate as `/dev/ttyACMx`).

## Build

**Requirements**

- CMake ≥ 3.10
- WCH RISC-V toolchain (`riscv-wch-elf-gcc`)
- `chprog` for flashing (WCH programmer tool)

**Configure**

```bash
cmake -S . -B build -DTOOLCHAIN_BIN_PATH=/path/to/riscv-wch-elf/bin
```

**Compile**

```bash
cmake --build build
```

Outputs are placed in `build/bin/`:
- `example.elf` — ELF with debug info
- `example.hex` — Intel HEX for flashing
- `example.bin` — raw binary for flashing

**Flash**

```bash
cmake --build build --target flash
```

This runs `chprog build/bin/example.bin` via the WCH-LinkE programmer.

> **Note:** CMake uses `GLOB_RECURSE` to collect sources. If you add new `.c`
> files, re-run the configure step before building.

## Target

- MCU: CH32X035 @ 48 MHz (RISC-V rv32imac)
- FPGA: Gowin with T80 Z80-compatible core running Pac-Man
- Build: CMake + riscv-wch-elf-gcc
