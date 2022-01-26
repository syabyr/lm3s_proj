#******************************************************************************
#
# Makefile - Rules for building the Sample SD Card Application.
#
# Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 9107 of the DK-LM3S9B96 Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S9B96

#
# Set the processor variant.
#
VARIANT=cm3

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
VPATH+=../../../third_party/fatfs/port
VPATH+=../../../third_party/fatfs/src
VPATH+=../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=..
IPATH+=../../..
IPATH+=../../../third_party

#
# The default rule, which causes the Sample SD Card Application to be built.
#
all: ${COMPILER}
all: ${COMPILER}/sd_card.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the Sample SD Card Application.
#
${COMPILER}/sd_card.axf: ${COMPILER}/cmdline.o
${COMPILER}/sd_card.axf: ${COMPILER}/ff.o
${COMPILER}/sd_card.axf: ${COMPILER}/kitronix320x240x16_ssd2119_8bit.o
${COMPILER}/sd_card.axf: ${COMPILER}/mmc-dk-lm3s9b96.o
${COMPILER}/sd_card.axf: ${COMPILER}/sd_card.o
${COMPILER}/sd_card.axf: ${COMPILER}/set_pinout.o
${COMPILER}/sd_card.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/sd_card.axf: ${COMPILER}/touch.o
${COMPILER}/sd_card.axf: ${COMPILER}/uartstdio.o
${COMPILER}/sd_card.axf: ${COMPILER}/ustdlib.o
${COMPILER}/sd_card.axf: ${ROOT}/grlib/${COMPILER}-cm3/libgr-cm3.a
${COMPILER}/sd_card.axf: ${ROOT}/driverlib/${COMPILER}-cm3/libdriver-cm3.a
${COMPILER}/sd_card.axf: sd_card.ld
SCATTERgcc_sd_card=sd_card.ld
ENTRY_sd_card=ResetISR
CFLAGSgcc=-DTARGET_IS_TEMPEST_RB1 -DUART_BUFFERED

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
