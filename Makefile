# Makefile for building STM32 stuff with their HAL library
# First I'm gonna get it working and then i try to generalize it a bit

PROGNAME	= stm_out

LIB_PFX		= /usr/local/STM32_SPL
SPL_DIR    	= $(LIB_PFX)/STM32F4xx_StdPeriph_Driver
CMSIS_DIR  	= $(LIB_PFX)/CMSIS

# Compiler tools
PREFIX     	= arm-none-eabi
CC         	= $(PREFIX)-gcc
AR         	= $(PREFIX)-ar
OBJCOPY    	= $(PREFIX)-objcopy
OBJDUMP    	= $(PREFIX)-objdump
SIZE       	= $(PREFIX)-size
GDB        	= $(PREFIX)-gdb

OCD_DIR    	= /Applications/GNU_ARM_Eclipse/OpenOCD/0.10.0-201510281129-dev
OCD        	= $(OCD_DIR)/bin/openocd
OCDARG		= -f board/stm32f4discovery.cfg

DEFINE       	= -DSTM32F401xx -DUSE_STDPERIPH_DRIVER -DSH_DEBUG

# Search paths
SRC       	= main.c utils.c gpio.c pwm.c i2c.c
SRC      	+= system_stm32f4xx.c
SRC      	+= interrupts.c
SRC		+= stm32f4xx_rcc.c stm32f4xx_tim.c stm32f4xx_exti.c
SRC		+= stm32f4xx_gpio.c stm32f4xx_syscfg.c misc.c
SRC		+= stm32f4xx_i2c.c

INC       	= -Isrc
INC      	+= -I$(CMSIS_DIR)/Include
INC		+= -I$(CMSIS_DIR)/Device/ST/STM32F4xx/Include
INC      	+= -I$(SPL_DIR)/inc

CFLAGS     	= -Wall -g -std=c99 -Os
CFLAGS    	+= -mlittle-endian -mcpu=cortex-m4 -mthumb
CFLAGS    	+= $(INC) $(DEFINE)

LFLAGS    	= -Wl,--gc-sections -Wl,-Map=$(PROGNAME).map -Tlinker/stm32f4_linker.ld
LFLAGS   	+= -lc -lrdimon -lgcc -Wall --specs=rdimon.specs -O3

VPATH      	= src
VPATH     	+= $(SPL_DIR)/src

OBJS       	= $(addprefix obj/,$(SRC:.c=.o))

.PHONY: all dir load debug clean

all: $(PROGNAME).elf

dir: obj
obj: 
	@echo "Creating  $@"
	@mkdir -p $@

obj/%.o: %.c | dir
	@echo "CC	-	$(notdir $<)"
	@$(CC) $(CFLAGS) -c -o $@ $< 

$(PROGNAME).elf: $(OBJS)
	@echo "LD	-	$(PROGNAME).elf"
	@$(CC) $(CFLAGS) $(LFLAGS) src/startup_stm32f401xc.s $^ -o $@
	@echo "OBJDUMP	-	$(PROGNAME).lst"
	@$(OBJDUMP) -St $(PROGNAME).elf >$(PROGNAME).lst
	@echo "Size of [$(PROGNAME).elf]"
	@$(SIZE) $(PROGNAME).elf

openocd:
	$(OCD) -s $(OCD_DIR) $(OCDARG)

load: all
	$(OCD) -s $(OCD_DIR) $(OCDARG) -c "program $(PROGNAME).elf verify reset"

debug:
	@if ! nc -z localhost 3333; then \
		echo "\nStart openocd first\n"; exit 1; \
	else \
		$(GDB)  -ex "target extended localhost:3333" \
			-ex "monitor arm semihosting enable" \
			-ex "monitor reset halt" \
			-ex "load" \
			-ex "monitor reset init" \
			-ex "break main" \
			$(GDBFLAGS) $(PROGNAME).elf; \
	fi

clean:
	@echo "Removing:	$(PROGNAME).elf"
	@rm -f $(PROGNAME).elf
	@echo "Removing:	$(PROGNAME).map"
	@rm -f $(PROGNAME).map
	@echo "Removing:	$(PROGNAME).lst"
	@rm -f $(PROGNAME).lst
	@echo "Removing:	obj"
	@rm -fr obj
