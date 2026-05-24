# VS Code STM32 Workflow

This project can be built and flashed without Keil MDK. The current setup uses:

- VS Code
- CMake
- Ninja
- Arm GNU Toolchain for arm-none-eabi
- xPack OpenOCD
- Cortex-Debug VS Code extension

## Build

Open this folder in VS Code:

```powershell
code D:\beifen\case\STM32_Project_3
```

Then run:

```text
Terminal > Run Build Task > build
```

The build writes these files under `build/debug/`:

- `MYPLAY.elf`
- `MYPLAY.hex`
- `MYPLAY.bin`
- `MYPLAY.map`

The same build can be run from PowerShell:

```powershell
cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE="$PWD/cmake/arm-none-eabi-gcc.cmake"
cmake --build build/debug --parallel
```

## Flash

Connect the board through ST-Link, then run:

```text
Terminal > Run Task > probe
```

If the probe succeeds, run:

```text
Terminal > Run Task > flash
```

The `flash` task builds first, then programs and verifies `build/debug/MYPLAY.elf`.

## Debug

Open the Run and Debug panel and start:

```text
Debug STM32F429 (OpenOCD)
```

The debug configuration starts OpenOCD, loads `build/debug/MYPLAY.elf`, and runs to `main`.

## Serial Port

Run this task to list Windows serial ports:

```text
Terminal > Run Task > serial: list ports
```

## ST-Link Driver

If OpenOCD prints `Error: open failed`, Windows is not exposing the ST-Link debug interface correctly, or the probe is not connected.

Use this order:

1. Connect ST-Link and power the board.
2. Run the `probe` task again.
3. If it still fails, open Zadig.
4. Select the ST-Link debug interface.
5. Install or replace the driver with WinUSB.
6. Do not replace the USB serial interface if you need virtual COM port logging.

## Tool Paths

This repository pins the local tool paths in `.vscode/settings.json` and `.vscode/tasks.json` so the workflow does not depend on a freshly refreshed Windows PATH.

If the tools are reinstalled to a different location, update:

- `cmake.cmakePath`
- `CMAKE_MAKE_PROGRAM`
- `ARM_GNU_TOOLCHAIN_BIN`
- `cortex-debug.openocdPath`
- `cortex-debug.armToolchainPath`
- `cortex-debug.gdbPath`
