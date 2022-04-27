AS = arm-none-eabi-as
CC = arm-none-eabi-gcc
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size


APP = HelloWorld
OUT = ./Build

# External Tools
PROGRAMMER = /opt/st/stm32cubeide_*/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.*/tools/bin/STM32_Programmer_CLI

# For serial monitor
TTY = /dev/ttyACM0

# STM32G0xx and CMSIS headers
STM32CUBEG0 = ../STM32CubeG0

SRC_DIR = ./Src
INC_DIR = ./Inc

# # # # # # # # # # # # # # # # # #
#
# USER APPLICATION
#
# # # # # # # # # # # # # # # # # #

INCLUDES += \
	$(INC_DIR) \

SOURCES += \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/startup.c \
	$(SRC_DIR)/interrupts.c \

ASM += \
	$(SRC_DIR)/core.s \


# # # # # # # # # # # # # # # # # #
#
# CMSIS
#
# # # # # # # # # # # # # # # # # #

INCLUDES += \
	$(STM32CUBEG0)/Drivers/CMSIS/Core/Include \
	$(STM32CUBEG0)/Drivers/CMSIS/Device/ST/STM32G0xx/Include \

# Linker script
LDSCRIPT = STM32G071.ld

# Compiler flags
CFLAGS += -mcpu=cortex-m0plus
CFLAGS += --specs=nano.specs
CFLAGS += -mthumb
CFLAGS += -MMD

# Assembler flags
ASFLAGS += -mcpu=cortex-m0plus
ASFLAGS += -mfloat-abi=soft
ASFLAGS += -mthumb

# Linker flags
LDFLAGS += --gc-sections
LDFLAGS += -Map="$(OUT)/$(APP).map"

# Optimisation flags
OFLAGS += -Og
OFLAGS += -gdwarf

# Warnings flags
WARNINGS += all

L = -Wl,
W = -W
I = -I
D = -D

OBJ_DIR = $(OUT)/Objects
OBJECTS = $(SOURCES:%.c=$(OBJ_DIR)/%.o) $(ASM:%.s=$(OBJ_DIR)/%.o)
DEPENDS = $(SOURCES:%.c=$(OBJ_DIR)/%.d)

.PHONY: all clean upload debug monitor
.SILENT:

all: $(OBJECTS)
	echo "Linking objects"
	mkdir -p "$(OUT)"
	$(CC) $(addprefix $L,$(LDFLAGS)) -T$(LDSCRIPT) $(CFLAGS) $(OBJECTS) -o "$(OUT)/$(APP).elf" -static -lc -lm
	$(OBJDUMP) -h -S "$(OUT)/$(APP).elf" > "$(OUT)/$(APP).list"
	$(OBJCOPY) -O binary "$(OUT)/$(APP).elf" "$(OUT)/$(APP).bin"
	$(SIZE) $(OUT)/$(APP).elf

clean:
	echo "Clean compilation artifacts"
	rm -rf "$(OUT)"

upload: all
	$(PROGRAMMER) -c port=swd -d "$(OUT)/$(APP).bin" 0x8000000 -v -hardRst

debug: $(OUT)/$(APP).elf
	st-util &
	gdb-multiarch -x init.gdb $<
	killall st-util

monitor:
	picocom -b 9600 $(TTY)

$(OBJ_DIR)/%.o: %.s
	echo "Building $<"
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.c
	echo "Building $<"
	mkdir -p $(@D)
	$(CC) $(addprefix $I,$(INCLUDES)) $(addprefix $D,$(DEFINES)) $(addprefix $W,$(WARNINGS)) $(CFLAGS) $(OFLAGS) -ffunction-sections -fdata-sections -c -o $@ $<

-include $(DEPENDS)
