驱动芯片:ILI9327
分辨率:240x3x400
管脚定义:


触摸屏部分:(4根)
2:Y-
3:X-
4:Y+
6:X+

数据格式定义:(2根)
IM0:1=00	:16bit总线
IM0:1=10	:18bit总线
IM0:1=11	:8bit总线


数据总线:(18根)
13-30


使用的图形驱动芯片是HX8352-A
HX8352-A支持3种接口模式,包括命令-参数模式,寄存器-内容模式.
接口模式可以通过外部管脚IFSEL0,P68,BS2~0来设置.

使用的分辨率:240(H) x RGB(H) x 400(V)
显示色彩模式:
	正常情况下:
	65,536(R(5),G(6),B(5))色彩
	262,144(R(6),G(6),B(6))色彩
这也是为什么最多用18根数据线,如果用8/16根数据线模式,可能最多只能65,536色彩,看下面数据传输的定义,如果能分两次或者三次传输,就能够使用最大色彩.

	空闲模式下,只有8色彩(R(1),G(1),B(1)).

芯片内部帧缓冲区大小:240(H) x 480(V) x 18bit




软件实现流程:

写数据:
将数据的高8bit和低8bit分别写到数据总线上,然后写入write使能信号.
读数据:
将数据总线改为输入模式,然后发送读信号,最后读取数据,再将数据总线改为输出模式.

