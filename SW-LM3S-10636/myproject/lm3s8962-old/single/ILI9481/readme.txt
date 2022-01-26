柔性接口在下方,默认情况下,从左到右,从上到下扫描.

设置使能读写范围:
	WriteCommand(0x2A);	//set column address
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x01);
	WriteData(0x3F);

	WriteCommand(0x2B);	//set page address
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x01);
	WriteData(0xDF);

