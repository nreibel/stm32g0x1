# STM32G0x1 Bare Metal C skeleton

Bare-metal example code to setup vector table, boot code, GPIO, SysTick and USART

Designed for NUCLEO-G071RB board

## How to build

Using arm-none-eabi toolchain

STM32CubeIDE must be installed to provide STM32_Programmer_CLI

st-util must be installed to provide debugging

- `make clean` : Clean compilation output
- `make all` : Build .bin and .elf
- `make upload` : upload .bin to target
- `make debug` : run gdb and connect to target
