#这是一个为LM3S系列Cortex-M3单片机编写的Makefile配置文件
#Auther:mybays@qq.com
#Date:Nov-18-2012

#单片机的型号
PART=LM3S9B96


#需要编译的C源文件
SRC = $(TARGET).c
SRC += startup_9b95.c
SRC += mydelay.c
SRC += ustdlib.c
SRC += uartstdio.c
SRC += client.c
SRC += 320x480_9B95.c
SRC += ds18b20.c
SRC += ${ROOT}/third_party/uip-1.0/uip/uip.c
SRC += ${ROOT}/third_party/uip-1.0/uip/uip_arp.c
SRC += ${ROOT}/third_party/uip-1.0/uip/psock.c
SRC += ${ROOT}/third_party/uip-1.0/uip/uip_timer.c
SRC += ${ROOT}/third_party/uip-1.0/apps/httpd/httpd.c
SRC += ${ROOT}/third_party/uip-1.0/apps/dhcpc/dhcpc.c


EXTRAPATH =
EXTRAPATH +=

#额外的CFLAG
#CFLAGSgcc=-DUART_BUFFERED -DTARGET_IS_DUSTDEVIL_RA0
#需要编译的C++源文件
CPPSRC = 
CPPSRC += 

#需要编译的汇编源文件
ASRC = 
ASRC += 

#开发环境的根目录
ROOT=../../..

include ${ROOT}/myproject/templates/makedefs
