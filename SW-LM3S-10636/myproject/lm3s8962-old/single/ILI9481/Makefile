
# Defines the part type that this project uses.
PART=LM3S8962

# Target file name (without extension).
TARGET = main

# The base directory for StellarisWare.
ROOT=../../..

# Include the common make definitions.
include ../makedefs

# Where to find source files that do not live in this directory.
VPATH=../drivers

# Where to find header files that do not live in the source directory.
IPATH=..
IPATH+=../../..

# The default rule, which causes the hello example to be built.
all: ${COMPILER}
all: ${COMPILER}/$(TARGET).axf
all:
	@cp ${COMPILER}/$(TARGET).axf ./
	@arm-stellaris-eabi-objdump -h -S -z main.axf > main.lss

# The rule to clean out all the build products.
clean:
	@rm -rf ${COMPILER} ${wildcard *~} $(TARGET).axf $(TARGET).lss

# The rule to create the target directory.
${COMPILER}:
	@mkdir ${COMPILER}

# Rules for building the hello example.
#
${COMPILER}/$(TARGET).axf: ${COMPILER}/$(TARGET).o
${COMPILER}/$(TARGET).axf: ${COMPILER}/ILI9481.o
#${COMPILER}/$(TARGET).axf: ${COMPILER}/ustdlib.o

${COMPILER}/$(TARGET).axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/$(TARGET).axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/$(TARGET).axf: $(TARGET).ld
SCATTERgcc_$(TARGET)=$(TARGET).ld
ENTRY_$(TARGET)=ResetISR

# Include the automatically generated dependency files.
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
