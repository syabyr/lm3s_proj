include ./makedefs

#定义删除命令
#
REMOVE_CMD = rm

#定义下载工具
#
FLASH_TOOL = OPENOCD

#定义是否使用thumb模式
#
USE_THUMB_MODE = YES

#定义目标板相关
#
MCU = cortex-m3
SUBMDL = $(PART)
CHIP = $(PART)
BOARD = EK-LM3S8962


#中断向量表地址
#
VECTOR_TABLE_LOCATION =

#输出目录
#
OUTDIR = gcc














#链接脚本的目录
#
LINKERSCRIPTPATH = .

#优化级别
#
OPT = s
#OPT = 3
#OPT = 2
#OPT = 1
#OPT = 0

#输出格式
#LOADFORMAT = ihex
#LOADFORMAT = binary
LOADFORMAT = both

#调试格式
#
#DEBUG = stabs
#DEBUG = dwarf-2

#C额外的定义
#
CDEFS = -DPART_${PART} -Dgcc -DLOADER

#ASM额外的定义
#
ADEFS = _D__ASSEMBLY__

#编译标准
#
CSTANDARD = -std=c99

#如果定义了额外的中断向量表
ifdef VECTOR_TABLE_LOCATION
CDEFS += -D$(VECTOR_TABLE_LOCATION)
ADEFS += -D$(VECTOR_TABLE_LOCATION)
endif

CDEFS += -D$(RUN_MODE) -D$(CHIP)
ADEFS += -D$(RUN_MODE) -D$(CHIP)

THUMB    = -mthumb


#定义CFLAGS
CFLAGS += -O$(OPT)
CFLAGS += -mcpu=$(MCU) $(THUMB_IW) 
CFLAGS += $(CDEFS)
CFLAGS += $(patsubst %,-I%,$(IPATH)) -I.
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wa,-adhlns=$(addprefix $(OUTDIR)/, $(notdir $(addsuffix .lst, $(basename $<))))
CFLAGS += -MD -MP -MF $(OUTDIR)/dep/$(@F).d

#flags only for C
CONLYFLAGS += $(CSTANDARD)

#定义CPPFLAGS
CPPFLAGS = -fno-rtti -fno-exceptions
CPPFLAGS += 

#定义ASFLAGS
ASFLAGS  = -mcpu=$(MCU) $(THUMB_IW) -I. -x assembler-with-cpp
ASFLAGS += $(ADEFS)
ASFLAGS += -Wa,-adhlns=$(addprefix $(OUTDIR)/, $(notdir $(addsuffix .lst, $(basename $<))))
ASFLAGS += -Wa,-g$(DEBUG)
ASFLAGS += $(patsubst %,-I%,$(IPATH))

MATH_LIB = -lm

LDFLAGS = -nostartfiles -Wl,-Map=$(OUTDIR)/$(TARGET).map,--cref,--gc-sections
LDFLAGS += $(patsubst %,-L%,$(EXTRA_LIBDIRS))
LDFLAGS += -lc
LDFLAGS += $(patsubst %,-l%,$(EXTRA_LIBS))
LDFLAGS += $(MATH_LIB)
LDFLAGS += $(CPLUSPLUS_LIB)
LDFLAGS += -lc -lgcc


#如果是内存调试
ifeq ($(RUN_MODE),RAM_RUN)
LDFLAGS +=-T$(LINKERSCRIPTPATH)/lm3s8962-ram.ld --entry ResetHandler
else 
LDFLAGS +=-T$(LINKERSCRIPTPATH)/lm3s8962-rom.ld --entry ResetHandler
endif


#OPENOCD设置
OOCD_LOADFILE+=$(OUTDIR)/$(TARGET).axf
OOCD_EXE=openocd
OOCD_CL=-d0
OOCD_CL+=-f interface/openocd-usb.cfg -f board/stm32f10x_128k_eval.cfg
OOCD_CL+=-c init -c targets
OOCD_CL+=-c "halt"
OOCD_CL+=-c "flash write_image erase $(OOCD_LOADFILE)" -c "verify_image $(OOCD_LOADFILE)"
OOCD_CL+=-c "reset run"
OOCD_CL+=-c shutdown

#定义编译相关
SHELL   = sh
CC      = $(TCHAIN_PREFIX)gcc
CPP     = $(TCHAIN_PREFIX)g++
AR      = $(TCHAIN_PREFIX)ar
OBJCOPY = $(TCHAIN_PREFIX)objcopy
OBJDUMP = $(TCHAIN_PREFIX)objdump
SIZE    = $(TCHAIN_PREFIX)size
NM      = $(TCHAIN_PREFIX)nm
COPY    = cp
REMOVE  = $(REMOVE_CMD) -f


# List of all source files.
ALLSRC     =$(ASRC) $(SRC) $(CPPSRC)
ALLSRCBASE = $(notdir $(basename $(ALLSRC)))
ALLOBJ     = $(addprefix $(OUTDIR)/, $(addsuffix .o, $(ALLSRCBASE)))

LSTFILES   = $(addprefix $(OUTDIR)/, $(addsuffix .lst, $(ALLSRCBASE)))
DEPFILES   = $(addprefix $(OUTDIR)/dep/, $(addsuffix .o.d, $(ALLSRCBASE)))


#编译从这里开始
all: begin gccversion build sizeafter end

begin:
	@echo -e "\033[43;30m -------- begin (mode: $(RUN_MODE)) --------\033[0m"

# Display compiler version information.
gccversion : 
	@echo -e "\033[47;32m GCC version: \033[0m"
	@$(CC) --version

ifeq ($(LOADFORMAT),ihex)
build: axf hex lss sym
else 
ifeq ($(LOADFORMAT),binary)
build: axf bin lss sym
else 
ifeq ($(LOADFORMAT),both)
build: axf hex bin lss sym
else 
$(error "$(MSG_FORMATERROR) $(FORMAT)")
endif
endif
endif

# Display sizes of sections.
ELFSIZE = $(SIZE) -A -x $(OUTDIR)/$(TARGET).axf
sizeafter:
	@echo -e "\033[43;30m Size after build:\033[0m"
	$(ELFSIZE)

end:
	@echo -e "\033[43;30m --------  end  --------\033[0m"


axf: $(OUTDIR)/$(TARGET).axf
	@if [ -f  $(TARGET).axf ]; then echo ;else $(COPY) $(OUTDIR)/$(TARGET).axf ./; fi
lss: $(OUTDIR)/$(TARGET).lss 
sym: $(OUTDIR)/$(TARGET).sym
hex: $(OUTDIR)/$(TARGET).hex
bin: $(OUTDIR)/$(TARGET).bin



# Create final output file (.hex) from ELF output file.
%.hex: %.axf
	@echo -e "\033[43;30mCreating load file:\033[0m" $@
	$(OBJCOPY) -O ihex $< $@
	
# Create final output file (.bin) from ELF output file.
%.bin: %.axf
	@echo -e "\033[43;30mCreating load file:\033[0m" $@
	$(OBJCOPY) -O binary $< $@

# Create extended listing file/disassambly from ELF output file.
# using objdump testing: option -C
%.lss: %.axf
	@echo -e "\033[43;30mCreating Extended Listing/Disassembly:\033[0m" $@
	$(OBJDUMP) -h -S -C -r $< > $@

# Create a symbol table from ELF output file.
%.sym: %.axf
	@echo -e "\033[43;30mCreating Symbol Table:\033[0m" $@
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).axf
.PRECIOUS : $(ALLOBJ)
%.axf:  $(ALLOBJ)
	@echo
	@echo -e "\033[43;30m Linking :\033[0m" $@
# use $(CC) for C-only projects or $(CPP) for C++-projects:
	$(CC) $(THUMB) $(CFLAGS) $(ALLOBJ) $(LIBA) --output $@ $(LDFLAGS)
#	$(CPP) $(THUMB) $(CFLAGS) $(ALLOBJ) --output $@ $(LDFLAGS)

# Program the device with Dominic Rath's OPENOCD in "batch-mode", needs cfg and "reset-script".
program: $(OUTDIR)/$(TARGET).elf
	@echo "\033[43;30mProgramming with OPENOCD\033[0m"
	$(OOCD_EXE) $(OOCD_CL)

# Assemble: create object files from assembler source files.
define ASSEMBLE_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo -e "\033[43;30mAssembling:\033[0m" $$< "->" $$@
	$(CC) -c $(THUMB) $$(ASFLAGS) $$< -o $$@ 
endef
$(foreach src, $(ASRC), $(eval $(call ASSEMBLE_TEMPLATE, $(src)))) 

# Compile: create object files from C source files.
define COMPILE_C_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo -e "\033[43;30m Compiling C :\033[0m" $$< "->" $$@
	$(CC) -c $(THUMB) $$(CFLAGS) $$(CONLYFLAGS) $$< -o $$@ 
endef
$(foreach src, $(SRC), $(eval $(call COMPILE_C_TEMPLATE, $(src)))) 

# Compile: create object files from C++ source files.
define COMPILE_CPP_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo -e "\033[43;30m Compiling C++ :\033[0m" $$< "->" $$@
	$(CC) -c $(THUMB) $$(CFLAGS) $$(CPPFLAGS) $$< -o $$@ 
endef
$(foreach src, $(CPPSRC), $(eval $(call COMPILE_CPP_TEMPLATE, $(src)))) 

# Compile: create assembler files from C source files. ARM/Thumb
$(SRC:.c=.s) : %.s : %.c
	@echo -e "\033[43;30mCreating asm-File from C-Source:\033[0m" $< to $@
	$(CC) $(THUMB) -S $(CFLAGS) $(CONLYFLAGS) $< -o $@

# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo $(MSG_CLEANING)
	$(REMOVE) $(OUTDIR)/$(TARGET).map
	$(REMOVE) $(OUTDIR)/$(TARGET).axf
	$(REMOVE) $(OUTDIR)/$(TARGET).hex
	$(REMOVE) $(OUTDIR)/$(TARGET).bin
	$(REMOVE) $(OUTDIR)/$(TARGET).sym
	$(REMOVE) $(OUTDIR)/$(TARGET).lss
	$(REMOVE) $(ALLOBJ)
	$(REMOVE) $(LSTFILES)
	$(REMOVE) $(DEPFILES)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRCARM:.c=.s)
	$(REMOVE) $(CPPSRC:.cpp=.s)
	$(REMOVE) $(CPPSRCARM:.cpp=.s)
	$(REMOVE) $(TARGET).axf
	$(REMOVE) -r gcc


# Create output files directory
$(shell mkdir $(OUTDIR) 2>/dev/null)

# Include the dependency files.
-include $(shell mkdir $(OUTDIR)/dep 2>/dev/null) $(wildcard $(OUTDIR)/dep/*)

# Listing of phony targets.
.PHONY : all begin finish end sizeafter gccversion \
build axf hex bin lss sym clean clean_list program
