#这是一个为LM3S系列Cortex-M3单片机编写的Makefile配置文件
#Auther:mybays@qq.com
#Date:Jun-28-2011


#单片机的型号
PART=LM3S9B96

#开发环境的根目录
ROOT=../../..


include ./makedefs


#编译源文件时的搜索路径,VPATH是Makefile里有效文件路径
VPATH = ./
VPATH += ${ROOT}/boot_loader
VPATH += ${ROOT}/driverlib
VPATH += ${ROOT}/drivers
VPATH+= ${ROOT}/drivers/${PART}
VPATH += ${ROOT}/grlib
VPATH += ${ROOT}/usblib
VPATH += ${ROOT}/utils

#需要编译的C源文件
SRC = $(TARGET).c
SRC += $(STARTUP)
SRC += uartstdio.c
SRC += usb_mouse_structs.c

#额外的CFLAG
CFLAGSgcc=-DUART_BUFFERED -DTARGET_IS_DUSTDEVIL_RA0
#需要编译的C++源文件
CPPSRC = 
CPPSRC += 

#需要编译的汇编源文件
ASRC = 
ASRC += 

#include要搜索的文件夹
IPATH = ./
IPATH += ${ROOT}
IPATH += ${ROOT}/boot_loader
IPATH += ${ROOT}/driverlib
IPATH += ${ROOT}/drivers
IPATH += ${ROOT}/drivers/${PART}
IPATH += ${ROOT}/grlib
IPATH += ${ROOT}/inc
IPATH += ${ROOT}/IQmath
IPATH += ${ROOT}/usblib
IPATH += ${ROOT}/utils

#打包库文件路径
EXTRA_LIBDIRS = ${ROOT}/lib
#额外的库文件
EXTRA_LIBS =
#静态库文件
LIBA = ${EXTRA_LIBDIRS}/libusb.a
LIBA += ${EXTRA_LIBDIRS}/libdriver.a
LIBA = ${ROOT}/driverlib/gcc-cm3/libdriver-cm3.a
LIBA += ${ROOT}/grlib/gcc-cm3/libgr-cm3.a
LIBA += ${ROOT}/usblib/gcc-cm3/libusb-cm3.a


#编译从这里开始
all: begin gccversion build sizeafter end

begin:
	@echo -e "\033[43;30m -------- begin (mode: $(RUN_MODE)) --------\033[0m"


#显示编译器的版本号
gccversion : 
	@echo -e "\033[47;32m GCC version: \033[0m"
	@$(CC) --version

#设置编译目标文件格式

build: axf hex bin lss sym

#显示最后目标文件占用的rom和ram
sizeafter:
	@echo -e "\033[43;30m Size after build:\033[0m"
	$(SIZE) -A -x $(OUTDIR)/$(TARGET).axf
	@echo -e "\033[43;30m (mode: $(RUN_MODE))\033[0m"
end:
	@echo -e "\033[43;30m --------  end  --------\033[0m"



axf: $(OUTDIR)/$(TARGET).axf
	@if [ -f  $(TARGET).axf ]; then echo ;else $(COPY) $(OUTDIR)/$(TARGET).axf ./; fi
lss: $(OUTDIR)/$(TARGET).lss 
sym: $(OUTDIR)/$(TARGET).sym
hex: $(OUTDIR)/$(TARGET).hex
bin: $(OUTDIR)/$(TARGET).bin



#生成hex文件
%.hex: %.axf
	@echo -e "\033[43;30mCreating load file:\033[0m" $@
	$(OBJCOPY) -O ihex $< $@
	
#生成bin文件
%.bin: %.axf
	@echo -e "\033[43;30mCreating load file:\033[0m" $@
	$(OBJCOPY) -O binary $< $@

#生成反汇编文件
# using objdump testing: option -C
%.lss: %.axf
	@echo -e "\033[43;30mCreating Extended Listing/Disassembly:\033[0m" $@
	$(OBJDUMP) -h -S -C -r $< > $@

#生成符号表
%.sym: %.axf
	@echo -e "\033[43;30mCreating Symbol Table:\033[0m" $@
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).axf
.PRECIOUS : $(ALLOBJ)



%.axf:  $(ALLOBJ)
	@echo
	@echo -e "\033[43;30m Linking :\033[0m" $@
	$(LD) $(LDFLAGS) -o $@ $(ALLOBJ) $(LIBA) ${LIBC} ${LIBM} ${LIBGCC}




#直接将固件编程到单片机上,待修改
program: $(TARGET).elf
	@echo "\033[43;30mProgramming with OPENOCD\033[0m"
	$(OOCD_EXE) $(OOCD_CL)


#由汇编文件生成目标文件
define ASSEMBLE_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo -e "\033[43;30mAssembling:\033[0m" $$< "->" $$@
	$(CC) -c $(THUMB) $$(ASFLAGS) $$< -o $$@ 
endef
$(foreach src, $(ASRC), $(eval $(call ASSEMBLE_TEMPLATE, $(src)))) 

#由c文件生成目标文件
define COMPILE_C_TEMPLATE
$(OUTDIR)/$(notdir $(basename $(1))).o : $(1)
	@echo -e "\033[43;30m Compiling C :\033[0m" $$< "->" $$@
	$(CC) -c $(CFLAGS) -Dgcc $$< -o $$@ 
endef
$(foreach src, $(SRC), $(eval $(call COMPILE_C_TEMPLATE, $(src)))) 


#由cpp文件生成目标文件
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
	$(REMOVE) $(TARGET).axf
	$(REMOVE) -r gcc


# Create output files directory
$(shell mkdir $(OUTDIR) 2>/dev/null)

# Include the dependency files.
-include $(shell mkdir $(OUTDIR)/dep 2>/dev/null) $(wildcard $(OUTDIR)/dep/*)
# Listing of phony targets.
.PHONY : all begin finish end sizeafter gccversion \
build axf hex bin lss sym clean clean_list program

