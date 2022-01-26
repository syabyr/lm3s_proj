Jun.22.2011
最新版本7243

图形库:SW-GRL-7243.exe
算术库:SW-IQMATH-7243.exe
USB库:SW-USBL-7243.exe

另外是各个器件单独的包:

SW-EK-LM3S811-7243.exe
SW-EK-LM3S3748-7243.exe
SW-EK-LM3S8962-7243.exe
SW-EK-LM3S9B92-7243.exe
SW-DK-LM3S9B96-7243.exe

这些exe都是rar格式的,可以直接用rar解压,将这些exe文件解压到当前目录,
可以看到解压后9B96占用的空间最多,262M,以9B96的文件为模板创建开发环境.








编译器安装:
下载最新的sourceryg++编译器:
sourceryg++-2011.03-68-arm-stellaris-eabi.bin
sourceryg++-2011.03-68-arm-stellaris-eabi.src.tar.bz2

首先安装bin文件:
./sourceryg++-2011.03-68-arm-stellaris-eabi.bin
一路默认就可以了.

此时交叉编译链被安装到了/home/$USER/CodeSourcery内,Sourcery_CodeBench_for_Stellaris_EABI
一般需要重启才能使环境变量生效,或者使用以下命令.
export PATH=/home/mybays/CodeSourcery/Sourcery_CodeBench_for_Stellaris_EABI/bin:${PATH}

不完全破解:
sourceryg++提供了源代码,可以进行部分编译.
MakefileVer-2011.Jun.22
提供了一个包,arm-stellaris-tools-ver-2011.Jun.22.tar.gz,解压后覆盖安装目录就能够进行gcc,as编译了,暂时不支持c++.

待交叉编译:
c++,g++,gdb,gdbui,sprite,ide

gdb似乎不用重新编译.



编译器设置好后,就可以进行编译测试了.
首先进入SW-USBL-7243,先执行make clean,再执行make,正常的话,会生成libusb.a

建立环境:
SW-DK-LM3S9B96-7243
stellaris/docs
stellaris/usblib
stellaris/grlib
stellaris/IQmath
IQmath不提供源码,只提供编译好的库,保留了gcc和sourcerygxx.
stellaris/windows_drivers
暂时没有提供windows下的交叉编译链
stellaris/driverlib		各个版本的driverlib都一样
stellaris/boot_loader		各个版本的bootloader都一样
stellaris/inc			除了lm3sxxxx.h不同外,其它的都一样,添加支持后,只许添加这个文件.
stellaris/utils			根据情况,不同的mcu都有不同的文件,9B96里是最全的
stellaris/tools
stellaris/third_party
stellaris/boards			各个版本内的Makefile都一样,但是子目录不一样,将各个子目录复制过来就可以了.
stellaris/examples


以上是官方gcc编译环境的设置

在此基础上,增加以下目录:
stellaris/lib		保存已经编译好的静态库
stellaris/drivers		保存各自板子的驱动
stellaris/myproject	自己的工程

myproject子目录里是以mcu的型号命令的目录,这个目录里才是具体的一个个的工程.

一个工程里面包含以下文件:
makedefs:由于每个工程里的源代码不一样,所以采用makedefs来记录不同的信息
Makefile:makefile是到一个模板目录的连接,一般不需要修改
lm3s9b95-ram.ld,ram连接脚本
lm3s9b95-rom.ld,rom和ram连接脚本
.gdbinit,gdb初始化脚本
startup_9b95.c,单片机启动脚本



myproject目录说明:
driver:驱动
include:头文件
lib:库
templates:模板
其它的为不同类型的工程目录


lm3s8962-ram.ld	:ram链接文件
lm3s8962-rom.ld	:rom链接文件
startup.c			:启动文件,中断函数需要添加重写
Makefile			:基本不需要修改
makedefs			:只要修改这个文件即可
gdbinit			:提供ram,flash两个函数
ek-lm3s8962.cfg	:LM3S8962的openocd配置文件
lm3s8962-ram.ld和lm3s8962-rom.ld
可以在执行make的时候定义:
make RUN_MODE=ROM_RUN
执行这一命令前要执行清除:make clean


