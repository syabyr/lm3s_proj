#这是一个为LM3S系列Cortex-M3单片机编写的Makefile配置文件
#Auther:mybays@qq.com
#Date:Jun-28-2011


#单片机的型号
PART=LM3S8962

#开发环境的根目录
ROOT=../../..

run=rom

#编译源文件时的搜索路径,VPATH是Makefile里有效文件路径
VPATH = ./
VPATH += ${ROOT}/boot_loader
VPATH += ${ROOT}/driverlib
VPATH += ${ROOT}/myproject/drivers
VPATH += ${ROOT}/myproject/drivers/${PART}/drivers
VPATH += ${ROOT}/grlib
VPATH += ${ROOT}/usblib
VPATH += ${ROOT}/utils
VPATH += ${ROOT}/myproject/templates

VPATH += ../lwip-1.3.2/apps/httpserver_raw

#需要编译的C源文件
SRC = $(TARGET).c
SRC += startup_${PART}.c
SRC += rit128x96x4.c
SRC += cgifuncs.c
SRC += httpd.c
SRC += io.c
SRC += lmi_fs.c
SRC += locator.c
SRC += lwiplib.c
SRC += ustdlib.c
#额外的CFLAG
#CFLAGSgcc=-DUART_BUFFERED -DTARGET_IS_DUSTDEVIL_RA0
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
IPATH += ${ROOT}/myproject/drivers
IPATH += ${ROOT}/myproject/drivers/${PART}
IPATH += ${ROOT}/grlib
IPATH += ${ROOT}/inc
IPATH += ${ROOT}/IQmath
IPATH += ${ROOT}/usblib
IPATH += ${ROOT}/utils
IPATH += ../lwip-1.3.2/apps
IPATH += ../lwip-1.3.2/ports/stellaris/include
IPATH += ../lwip-1.3.2/src/include
IPATH += ../lwip-1.3.2/src/include/ipv4


#打包库文件路径
EXTRA_LIBDIRS = ${ROOT}/lib
#额外的库文件
EXTRA_LIBS =
#静态库文件
LIBA = ${EXTRA_LIBDIRS}/libdriver.a
#LIBA += ${EXTRA_LIBDIRS}/libusb.a


include ${ROOT}/myproject/templates/makedefs
