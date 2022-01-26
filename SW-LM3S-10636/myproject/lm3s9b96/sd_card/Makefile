#这是一个为LM3S系列Cortex-M3单片机编写的Makefile配置文件
#Auther:mybays@qq.com
#Date:Nov-18-2012

#单片机的型号
PART=LM3S9B96


#需要编译的C源文件
SRC = $(TARGET).c
SRC += startup_${PART}.c
SRC += mydelay.c
SRC += ustdlib.c
SRC += uartstdio.c


SRC += cmdline.c
SRC += ff.c
SRC += kitronix320x240x16_ssd2119_8bit.c
SRC += mmc-dk-lm3s9b96.c
SRC += set_pinout.c
SRC += touch.c


EXTRAPATH = ${ROOT}/third_party/fatfs/port
EXTRAPATH +=${ROOT}/third_party/fatfs/src
EXTRAPATH += ${ROOT}/boards/dk-lm3s9b96
EXTRAPATH += ${ROOT}/boards/dk-lm3s9b96/drivers
EXTRAPATH += ${ROOT}
#额外的CFLAG
CFLAGSgcc=-DUART_BUFFERED -DTARGET_IS_DUSTDEVIL_RA0 -DTARGET_IS_TEMPEST_RB1 -DUART_BUFFERED
#需要编译的C++源文件
CPPSRC = 
CPPSRC += 

#需要编译的汇编源文件
ASRC = 
ASRC += 

#开发环境的根目录
ROOT=../../..

include ${ROOT}/myproject/templates/makedefs
