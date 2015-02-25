# Makefile for building STM32 stuff with their HAL library
# First I'm gonna get it working and then i try to generalize it a bit

TOOL_PFX = arm-none-eabi
CC = $(TOOL_PFX)-gcc
AS = $(TOOL_PFX)-as
OBJCOPY = $(TOOL_PFX)-objcopy
OBJDUMP	= $(TOOL_PFX)-objdump
GDB	= $(TOOL_PFX)-gdb
SIZE = $(TOOL_PFX)-size

HAL_SRC = /usr/local/lib/arm/hal/source
HAL_INC = /usr/local/lib/arm/hal/include
CMSIS = /usr/local/lib/arm/CMSIS

BIN = main.hex
ELF = main.elf

LINKER = linker
LK = $(LINKER)/stm32f4_linker.ld
STARTUP = startup
SRC=src
BUILD=build

CFLAGS = -Wall -g -std=c99
CFLAGS += -mthumb -mcpu=cortex-m4
CFLAGS += -I$(HAL_INC) -I$(CMSIS)

LFLAGS = -T$(LK) -Wall -mthumb -mcpu=cortex-m4 -lgcc -lc -lrdimon 

S = $(wildcard $(SRC)/*.c)
S += $(wildcard $(STARTUP)/*.s)
H = $(wildcard $(SRC)/*.h)
O = $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(patsubst $(STARTUP)/%.s, $(BUILD)/%.o, $(S)))


all: $(BIN)

$(BIN): $(ELF)
	@$(OBJCOPY) -Oihex $(ELF) $(BIN)
	@echo "Elf is ready to load!!!"

$(BUILD)/%.o: $(SRC)/%.s
	@$(AS) -o $@ $<
	@echo "Assembly of $< finished"

$(BUILD)/%.o: $(SRC)/%.c
#compile all files first
	@mkdir -p $(BUILD)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo "Compilation of $< finished"

$(BUILD)/%.o: $(STARTUP)/%.s
#compile all files first
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo "Compilation of $< finished"

$(ELF): $(O)
	@echo "Linking..."
	@$(CC) $(LFLAGS) $(O) -o $(ELF)
	@echo "Linking done"

.PHONY: clean
clean:
	@echo "Deleting *.o files"
	@$(RM) $(BUILD)/*.o
	@echo "Deleting elf and hex files"
	@$(RM) $(BIN) $(ELF)

#just for Makefile debugging
.PHONY: show
show:
	@echo "Source dir: $(SRC)"
	@echo "Build dir: $(BUILD)"
	@echo "source files: $(S)"
	@echo "header files: $(H)"
	@echo "obj files: $(O)"