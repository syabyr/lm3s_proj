#Makefile for LM3S series Cortex-M3
#Auther:mybays@qq.com
#Date:Jun.26-2011


#定义器件使用的器件
PART=LM3S9B95

#开发环境的主目录
ROOT=../../..

#编译源文件时的搜索路径,VPATH是Makefile里有效文件路径,如果有第三方源文件,需要继续添加
VPATH = ./
VPATH += ${ROOT}/boot_loader
VPATH += ${ROOT}/driverlib
VPATH += ${ROOT}/drivers
VPATH+= ${ROOT}/drivers/${PART}
VPATH += ${ROOT}/grlib
VPATH += ${ROOT}/usblib
VPATH += ${ROOT}/utils

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


#需要编译的C源文件
SRC = $(TARGET).c
SRC += $(STARTUP)
SRC += uartstdio.c
SRC += buttons.c
SRC += formike128x128x16.c
SRC += usb_mouse_structs.c


