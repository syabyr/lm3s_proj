
# Defines the part type that this project uses.
#
PART=LM3S8962


TARGET = main

#
# The base directory for StellarisWare.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=../drivers
VPATH+=../../../third_party/lwip-1.3.2/apps/httpserver_raw
VPATH+=../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=..
IPATH+=../../..
IPATH+=../../../third_party/lwip-1.3.2/apps
IPATH+=../../../third_party/lwip-1.3.2/ports/stellaris/include
IPATH+=../../../third_party/lwip-1.3.2/src/include
IPATH+=../../../third_party/lwip-1.3.2/src/include/ipv4

#
# The default rule, which causes the Sample Ethernet I/O Control Application using lwIP 1.3.2 to be built.
#
all: ${COMPILER}
all: ${COMPILER}/main.axf
all:
	@cp ${COMPILER}/$(TARGET).axf ./
	@arm-stellaris-eabi-objdump -h -S -z main.axf > main.lss
#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~} main.axf

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir ${COMPILER}
#
# Rules for building the Sample Ethernet I/O Control Application using lwIP 1.3.2.
#
${COMPILER}/main.axf: ${COMPILER}/cgifuncs.o
${COMPILER}/main.axf: ${COMPILER}/main.o
${COMPILER}/main.axf: ${COMPILER}/httpd.o
${COMPILER}/main.axf: ${COMPILER}/io.o
${COMPILER}/main.axf: ${COMPILER}/lmi_fs.o
${COMPILER}/main.axf: ${COMPILER}/locator.o
${COMPILER}/main.axf: ${COMPILER}/lwiplib.o
${COMPILER}/main.axf: ${COMPILER}/rit128x96x4.o
${COMPILER}/main.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/main.axf: ${COMPILER}/ustdlib.o
${COMPILER}/main.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/main.axf: main.ld
SCATTERgcc_main=main.ld
ENTRY_main=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
