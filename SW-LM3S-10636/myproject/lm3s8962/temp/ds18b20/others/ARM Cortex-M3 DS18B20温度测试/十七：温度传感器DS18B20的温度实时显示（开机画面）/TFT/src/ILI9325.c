#include "stm32f10x_lib.h"
#include "ILI9325.h"
#include "fonts.h"
//#include "GB2312.h"

GPIO_InitTypeDef GPIO_InitStructure;

//Red Gray scale灰阶(65K)
const unsigned char RedGrayScale[]={
  0x00,0x00, 0x08,0x00, 0x10,0x00, 0x18,0x00, 0x20,0x00, 0x28,0x00, 0x30,0x00, 0x38,0x00,
  0x40,0x00, 0x48,0x00, 0x50,0x00, 0x58,0x00, 0x60,0x00, 0x68,0x00, 0x70,0x00, 0x78,0x00,
  0x80,0x00, 0x88,0x00, 0x90,0x00, 0x98,0x00, 0xA0,0x00, 0xA8,0x00, 0xB0,0x00, 0xB8,0x00,
  0xC0,0x00, 0xC8,0x00, 0xD0,0x00, 0xD8,0x00, 0xE0,0x00, 0xE8,0x00, 0xF0,0x00, 0xF8,0x00,
};
//Green Gray scale灰阶(65K)
const unsigned char  GreenGrayScale[]={
  0x00,0x00, 0x00,0x60, 0x00,0xA0, 0x00,0xE0, 0x01,0x00, 0x01,0x60, 0x01,0xA0, 0x01,0xE0,
  0x02,0x00, 0x02,0x60, 0x02,0xA0, 0x02,0xE0, 0x03,0x00, 0x03,0x60, 0x03,0xA0, 0x03,0xE0,
  0x04,0x00, 0x04,0x60, 0x04,0xA0, 0x04,0xE0, 0x05,0x00, 0x05,0x60, 0x05,0xA0, 0x05,0xE0,
  0x06,0x00, 0x06,0x60, 0x06,0xA0, 0x06,0xE0, 0x07,0x00, 0x07,0x60, 0x07,0xA0, 0x07,0xE0,
 };

//Blue Gray scale灰阶(65K)
const unsigned char  BlueGrayScale[]={
  0x00,0x00, 0x00,0x01, 0x00,0x02, 0x00,0x03, 0x00,0x04, 0x00,0x05, 0x00,0x06, 0x00,0x07,
  0x00,0x08, 0x00,0x09, 0x00,0x0A, 0x00,0x0B, 0x00,0x0C, 0x00,0x0D, 0x00,0x0E, 0x00,0x0F,
  0x00,0x10, 0x00,0x11, 0x00,0x12, 0x00,0x13, 0x00,0x14, 0x00,0x15, 0x00,0x16, 0x00,0x17,
  0x00,0x18, 0x00,0x19, 0x00,0x1A, 0x00,0x1B, 0x00,0x1C, 0x00,0x1D, 0x00,0x1E, 0x00,0x1F,
 };

//White Gray scale灰阶(65K)
const unsigned char  WhiteGrayScale[]={
  0x00,0x00, 0x08,0x61, 0x10,0xA2, 0x18,0xE3, 0x21,0x04, 0x29,0x65, 0x31,0xA6, 0x39,0xE7,
  0x42,0x08, 0x4A,0x69, 0x52,0xAA, 0x5A,0xEB, 0x63,0x0C, 0x6B,0x6D, 0x73,0xAE, 0x7B,0xEF,
  0x84,0x10, 0x8C,0x71, 0x94,0xB2, 0x9C,0xF3, 0xA5,0x14, 0xAD,0x75, 0xB5,0xB6, 0xBD,0xF7,
  0xC6,0x18, 0xCE,0x79, 0xD6,0xBA, 0xDE,0xFB, 0xE7,0x1C, 0xEF,0x7D, 0xF7,0xBE, 0xFF,0xFF,
 };

/*******************************************************************************
* Function Name  : ILI9325_CtrlLinesConfig
* Description    : Configures LCD control lines in Output Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_Reset(void)
{
  //对TFT进行复位一次
  ILI9325_RESET_HIGH();
  ILI9325_Delay(10);
  ILI9325_RESET_LOW();                               
  ILI9325_Delay(100);
  ILI9325_RESET_HIGH();                    
  ILI9325_Delay(100);
}

//延时功能函数
void  ILI9325_Delay (unsigned long nCount)
{
  int  i;
  for (i = 0; i < nCount * 250; i++) 
  { }
}

/*******************************************************************************
* Function Name  : ILI9325_CtrlLinesConfig
* Description    : Configures LCD control lines in Output Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_CtrlLinesConfig(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
  
  /* Configure PD.7--PD.11 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIOD_LCD_CS | GPIOD_LCD_RS | GPIOD_LCD_WR | GPIOD_LCD_RD |
                                GPIOD_LCD_RESET ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Configure PE.00 -- PE.15 as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : ILI9325_DisplayOff
* Description    : Disables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DisplayOff(void)
{
  ILI9325_WriteReg(R7, 0x0031);
  ILI9325_Delay(4);                
  ILI9325_WriteReg(R7, 0x0030);
  ILI9325_Delay(4);
  ILI9325_WriteReg(R7, 0x0000);
  ILI9325_WriteReg(R10, 0x0000);
  ILI9325_WriteReg(R12, 0x0000); 
}

/*******************************************************************************
* Function Name  : ILI9325_DisplayOn
* Description    : Enables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DisplayOn(void)
{
  /***************Power Control*********************/  
  ILI9325_WriteReg(R10, 0x0000);
  ILI9325_WriteReg(R11, 0x0000);
  ILI9325_WriteReg(R12, 0x0000);
  ILI9325_WriteReg(R13, 0x0000);
  ILI9325_Delay(200);
  ILI9325_WriteReg(R10, 0x1290);
  ILI9325_WriteReg(R11, 0x0227);
  ILI9325_Delay(50);
  ILI9325_WriteReg(R12, 0x001b);
  ILI9325_WriteReg(R13, 0x1900);
  
  ILI9325_WriteReg(R7, 0x0101);
  ILI9325_Delay(6);               
  ILI9325_WriteReg(R7, 0x0121);
  ILI9325_WriteReg(R7, 0x0123);
  ILI9325_Delay(6);
  ILI9325_WriteReg(R7, 0x0133); 
  ILI9325_Delay(50);
}

/*******************************************************************************
* Function Name  : ILI9325_PowerOn
* Description    : Power on the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_PowerOn(void)
{
 
}

/*******************************************************************************
* Function Name  : ILI9325_WriteGRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_WriteGRAM(unsigned short RGB_Code)
{
 ILI9325_WriteReg(R22,RGB_Code);
}

/*******************************************************************************
* Function Name  : ILI9325_WriteData
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void ILI9325_WriteData(unsigned short Data)
{
  ILI9325_RS_HIGH();
  ILI9325_CS_LOW();
  ILI9325_RD_HIGH();
  GPIO_Write(GPIOE,Data);
  ILI9325_WR_LOW();
  ILI9325_WR_HIGH();
  ILI9325_CS_HIGH();
}
/*******************************************************************************
* Function Name  : ILI9325_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_WriteReg(unsigned char LCD_Reg, unsigned short LCD_RegValue)
{
  //写索引寄存器
  ILI9325_RS_LOW();
  ILI9325_CS_LOW();
  ILI9325_RD_HIGH();
  GPIO_Write(GPIOE,LCD_Reg);
  ILI9325_WR_LOW();
  ILI9325_WR_HIGH();
  ILI9325_CS_HIGH();
  //向所寻寄存器中写入值
  ILI9325_RS_HIGH();
  ILI9325_CS_LOW();
  ILI9325_RD_HIGH();
  GPIO_Write(GPIOE,LCD_RegValue);
  ILI9325_WR_LOW();
  ILI9325_WR_HIGH();
  ILI9325_CS_HIGH();
}

/*******************************************************************************
* Function Name  : ILI9325_WriteIndex
* Description    : 
* Input          : 
*                  
*               
* Output         : 
* Return         : 
*******************************************************************************/
void ILI9325_WriteIndex(unsigned char Index)
{
  //写索引寄存器
  ILI9325_RS_LOW();
  ILI9325_CS_LOW();
  ILI9325_RD_HIGH();
  GPIO_Write(GPIOE,Index);
  ILI9325_WR_LOW();
  ILI9325_WR_HIGH();
  ILI9325_CS_HIGH();
}
/*******************************************************************************
* Function Name  : ILI9325_DrawCircle
* Description    : Displays a circle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*               
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DrawCircle(unsigned short Xpos, unsigned short Ypos, unsigned short Radius)
{
  s32  D;/* Decision Variable */
  u32  CurX;/* Current X Value */
  u32  CurY;/* Current Y Value */

  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;

  while (CurX <= CurY)
  {
    ILI9325_SetCursor(Xpos + CurX, Ypos + CurY);
    ILI9325_WriteGRAM(TextColor);
    ILI9325_SetCursor(Xpos + CurX, Ypos - CurY);
    ILI9325_WriteGRAM(TextColor);
    ILI9325_SetCursor(Xpos - CurX, Ypos + CurY);
    ILI9325_WriteGRAM(TextColor);
    ILI9325_SetCursor(Xpos - CurX, Ypos - CurY);
    ILI9325_WriteGRAM(TextColor);
    
    ILI9325_SetCursor(Xpos + CurY, Ypos + CurX);
    ILI9325_WriteGRAM(TextColor);
    ILI9325_SetCursor(Xpos + CurY, Ypos - CurX);
    ILI9325_WriteGRAM(TextColor);
    ILI9325_SetCursor(Xpos - CurY, Ypos + CurX);
    ILI9325_WriteGRAM(TextColor);
    ILI9325_SetCursor(Xpos - CurY, Ypos - CurX);
    ILI9325_WriteGRAM(TextColor);

    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/*******************************************************************************
* Function Name  : ILI9325_DrawRect
* Description    : Displays a rectangle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Height: display rectangle height.
*                  - Width: display rectangle width.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DrawRect(unsigned short Xpos, unsigned short Ypos, unsigned short Width, unsigned short Height )
{
  
  ILI9325_DrawLine(Xpos, Ypos, Width, Horizontal);//"上-"
  ILI9325_DrawLine(Xpos, (Ypos + Height-1), Width, Horizontal);//"下-"
  
  ILI9325_DrawLine(Xpos , (Ypos+1), (Height-2), Vertical);//"左|"
  ILI9325_DrawLine((Xpos + Width-1), (Ypos+1) , (Height-2), Vertical);//"右|"
}

/*******************************************************************************
* Function Name  : ILI9325_DrawLine
* Description    : Displays a line.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Length: line length.
*                  - Direction: line direction.
*                    This parameter can be one of the following values: Vertical
*                    or Horizontal.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DrawLine(unsigned short Xpos, unsigned short Ypos, unsigned short Length, unsigned short Direction)
{
  u16 i = 0;

  ILI9325_SetCursor(Xpos, Ypos);

  if(Direction == Horizontal)
  {
    for(i = 0; i < Length; i++)
    {
      ILI9325_WriteGRAM(TextColor);
      Xpos++;
      ILI9325_SetCursor(Xpos, Ypos);
    }
  }
  else
  {
   for(i = 0; i < Length; i++)
    {

      ILI9325_WriteGRAM(TextColor);
      Ypos++;
      ILI9325_SetCursor(Xpos, Ypos);
    }
  }
}

/*******************************************************************************
* Function Name  : ILI9325_SetDisplayWindow
* Description    : Sets a display window
* Input          : - Xpos: specifies the X buttom left position.
*                  - Ypos: specifies the Y buttom left position.
*                  - Height: display window height.
*                  - Width: display window width.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_SetDisplayWindow(unsigned short Xpos, unsigned short Ypos, unsigned short Width,  unsigned short Height)
{ 
  if((Xpos<240)&&(Ypos<320))
  {
    ILI9325_SetCursor(Xpos,Ypos);
    ILI9325_WriteReg(R50, Xpos);
    ILI9325_WriteReg(R51, (Xpos+Width-1));
    ILI9325_WriteReg(R52, Ypos);
    ILI9325_WriteReg(R53, (Ypos+Height-1));
  }
}

/*******************************************************************************
* Function Name  : ILI9325_DisplayString
* Description    : Displays a maximum of 600 char on the LCD.
* Input          : - Line: the starting Line where to display the character shape.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..19
*                  - *ptr: pointer to string to display on LCD.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DisplayString(unsigned char Line, unsigned char *ptr)
{
  u16 index = 0;
  u16 refcolumn = 0;
  u32 length = 0;

  /* Get the string length */
  length = StrLength(ptr);
  if(length > 600)//当字符总数超过600个时，在第一行显示提示信息"String too long"
  {
    /* Set the Cursor position */
    ILI9325_SetCursor(0,Line);
    /* Clear the Selected Line */
    ILI9325_ClearLine(Line);
    ILI9325_DisplayStringLine(Line, "        String too long       ");
  }
  else
  {
    /* Set the Cursor position */
    ILI9325_SetCursor(0,Line);
    /* Clear the Selected Line */
    ILI9325_ClearLine(Line);

    while(length--)
    {
      if(index == 30)//一行写慢了，继续下一行写
      {
        refcolumn = 0;
        Line += 16;
        index = 0;
      }
      /* Display one character on LCD */
      ILI9325_DisplayChar(Line, refcolumn, *ptr);

      /* Increment character number in one line */
      index++;

      /* Decrement the column position by 16 */
      refcolumn += 8;
      /* Point on the next character */
      ptr++;
    }
  }
}

/*******************************************************************************
* Function Name  : ILI9325_DisplayStringLine
* Description    : Displays a maximum of 30 char on the LCD.
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..19
*                  - *ptr: pointer to string to display on LCD.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DisplayStringLine(unsigned char Line, unsigned char *ptr)
{
  u32 i = 0;
  u16 refcolumn=0;
  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 30))
  {
    /* Display one character on LCD */
    ILI9325_DrawChar(refcolumn,(Line*16), ptr);
    /* Decrement the column position by 16 */
    refcolumn += 8;
    /* Point on the next character */
    ptr++;
    /* Increment the character counter */
    i++;
  }
}

/*******************************************************************************
* Function Name  : ILI9325_DisplayChar
* Description    : Displays one character .
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..19
*                  - Column: start column address.
*                  - Ascii: character ascii code
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DisplayChar(unsigned short Line, unsigned short Column, unsigned char Ascii)
{
  ILI9325_DrawChar(Column*8,Line*16, &Ascii);
}

/*******************************************************************************
* Function Name  : ILI9325_DrawChar
* Description    : Draws a character on LCD.
* Input          : - Xpos: the Line where to display the character shape.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..19
*                  - Ypos: start column address.
*                  - c: pointer to the character data.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DrawChar(unsigned short Xpos, unsigned short Ypos, unsigned char *c)
{
  u8 i=0,j=0;
  u8 str=0;
  u16 OffSet=0;
  
  OffSet = (*c - 32)*16;//寻找ASCII_Table[]中相应字符的点阵数据
          /*pAscii传过来的是该字符的ASCII码数字表示（参看ASCII表，有严格
          顺序表示），减32是因为第0~31号是控制字符，第32号为“空格”字符
          除以16是因为在ASCII_Table[]中的每个字符点阵为16个字节表示*/
  
  for(i=0;i<16;i++)
  {
    ILI9325_SetCursor(Xpos, Ypos+i);
    str = *(ASCII_Table + OffSet + i);
    for(j=0;j<8;j++)
    {
      if(str & (0x80>>j))
      {
        ILI9325_WriteGRAM(TextColor);
      }
      else
      {
        ILI9325_WriteGRAM(BackColor);
      }
    }
  }

}

/*******************************************************************************
* Function Name  : ILI9325_DrawCharColor
* Description    : Draws a character on LCD.
* Input          : - Xpos: the Line where to display the character shape.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..19
*                  - Ypos: start column address.
*                  - c: pointer to the character data.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_DrawCharColor(unsigned short Xpos, unsigned short Ypos, unsigned char *c,unsigned short CharColor,unsigned short BkColor)
{
  u8 i=0,j=0;
  u8 str=0;
  u16 OffSet=0;
  
  OffSet = (*c - 32)*16;//寻找ASCII_Table[]中相应字符的点阵数据
          /*pAscii传过来的是该字符的ASCII码数字表示（参看ASCII表，有严格
          顺序表示），减32是因为第0~31号是控制字符，第32号为“空格”字符
          除以16是因为在ASCII_Table[]中的每个字符点阵为16个字节表示*/
  
  for(i=0;i<16;i++)
  {
    ILI9325_SetCursor(Xpos, Ypos+i);
    str = *(ASCII_Table + OffSet + i);
    for(j=0;j<8;j++)
    {
      if(str & (0x80>>j))
      {
        ILI9325_WriteGRAM(CharColor);
      }
      else
      {
        ILI9325_WriteGRAM(BkColor);
      }
    }
  }

}
/*******************************************************************************
* Function Name  : ILI9325_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_SetCursor(unsigned short Xpos, unsigned short Ypos)
{
  ILI9325_WriteReg(R20, Xpos);
  ILI9325_WriteReg(R21, Ypos );
  
}

/*******************************************************************************
* Function Name  : ILI9325_Clear
* Description    : Clears the hole LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_Clear(unsigned short COLOR)
{
  u16 i=0,j=0;

  ILI9325_SetCursor(0, 0);

  for(i=0;i<320;i++)
  {
    for(j=0;j<240;j++)
    {
      ILI9325_WriteGRAM(COLOR);
    }
  }  
}

/*******************************************************************************
* Function Name  : ILI9325_ClearLine
* Description    : Clears the selected line.
* Input          : - Line: the Line to be cleared.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..19
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_ClearLine(unsigned char Line)
{
  ILI9325_DisplayStringLine(Line, "                              ");
}

/*******************************************************************************
* Function Name  : ILI9325_SetTextColor
* Description    : Sets the Text color.
* Input          : - Color: specifies the Text color code RGB(5-6-5).
* Output         : - TextColor: Text color global variable used by LCD_DrawChar
*                  and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void ILI9325_SetTextColor(volatile unsigned short Color)
{
  TextColor = Color;
}

/*******************************************************************************
* Function Name  : ILI9325_SetBackColor
* Description    : Sets the Background color.
* Input          : - Color: specifies the Background color code RGB(5-6-5).
* Output         : - BackColor: Background color global variable used by
*                  LCD_DrawChar and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void ILI9325_SetBackColor(volatile unsigned short Color)
{
  BackColor = Color;
}

/*******************************************************************************
* Function Name  : ILI9325_Init
* Description    : Initializes LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_Init(void)
{ 
  //Driver IC:ILI9325
  
  ILI9325_CtrlLinesConfig();//TFT的系统接口配置（本TFT模块采用的是i80 16-bit System Bus）
  ILI9325_Reset();//复位一次

  /*其中的SM位配合R96中的GS位，来决定图像刷新方向。本配置为从上到下
  SM GS=00从下到上  SM GS=01从上到下  SM GS=10 从右到左  SM GS=11 从左到右*/
  ILI9325_WriteReg(R1, 0x0100);//R1:驱动输出控制
  
  ILI9325_WriteReg(R2, 0x0700);//line inversion
  /*GRAM水平方向更新*/
  ILI9325_WriteReg(R3, 0x1030);//R3：数据进入模式设置
 
  //保持原图像大小
  ILI9325_WriteReg(R4, 0x0000);//R4：可将原图像缩小到1/2或1/4来显示
  
/*****************Display Control*******************/
  ILI9325_WriteReg(R8, 0x0202);
  ILI9325_WriteReg(R9, 0x0000);
  ILI9325_WriteReg(R0A, 0x0000);
  ILI9325_WriteReg(R0C, 0x0000);
  ILI9325_WriteReg(R0D, 0x0000);
  ILI9325_WriteReg(R0F, 0x0000);
  
/***************Power Control*********************/  
  ILI9325_WriteReg(R10, 0x0000);
  ILI9325_WriteReg(R11, 0x0000);
  ILI9325_WriteReg(R12, 0x0000);
  ILI9325_WriteReg(R13, 0x0000);
  ILI9325_Delay(200);
  ILI9325_WriteReg(R10, 0x1290);
  ILI9325_WriteReg(R11, 0x0227);
  ILI9325_Delay(50);
  ILI9325_WriteReg(R12, 0x001b);
  ILI9325_WriteReg(R13, 0x1900);
  ILI9325_WriteReg(R29, 0x0008);
  ILI9325_WriteReg(R2B, 0x000c);
  ILI9325_Delay(50);
  
  //默认显示坐标（0，0)
  ILI9325_WriteReg(R20, 0x0000);
  ILI9325_WriteReg(R21, 0x0000);
  
/**************Gamma Control***********/  
  ILI9325_WriteReg(R30, 0x0000);
  ILI9325_WriteReg(R31, 0x0406);
  ILI9325_WriteReg(R32, 0x0004);
  ILI9325_WriteReg(R35, 0x0305);
  ILI9325_WriteReg(R36, 0x0004);
  ILI9325_WriteReg(R37, 0x0207);
  ILI9325_WriteReg(R38, 0x0103);
  ILI9325_WriteReg(R39, 0x0707);
  ILI9325_WriteReg(R3C, 0x0503);
  ILI9325_WriteReg(R3D, 0x0004);

/********Sets display window,默认为整个屏幕********/
  ILI9325_WriteReg(R50, 0x0000);
  ILI9325_WriteReg(R51, 0x00ef);
  ILI9325_WriteReg(R52, 0x0000);
  ILI9325_WriteReg(R53, 0x013f);

/*******Gate Scan Control*************************/  
  ILI9325_WriteReg(R60, 0xa700);
  ILI9325_WriteReg(R61, 0x0001);
  ILI9325_WriteReg(R6A, 0x0000);
/********Partial Image Display Position Control***/
  ILI9325_WriteReg(R80, 0x0000);
  ILI9325_WriteReg(R81, 0x0000);
  ILI9325_WriteReg(R82, 0x0000);
  ILI9325_WriteReg(R83, 0x0000);
  ILI9325_WriteReg(R84, 0x0000);
  ILI9325_WriteReg(R85, 0x0000);
/*******Panel Interface Control******************/ 
  ILI9325_WriteReg(R90, 0x0010);
  ILI9325_WriteReg(R92, 0x0600);
  
  ILI9325_WriteReg(R7, 0x0133);
  
  ILI9325_Delay(300);
}

/*******************************************************************************
* Function Name  : StrLength
* Description    : Returns length of string.
* Input          : - Str: Character Pointer.
* Output         : None
* Return         : String length.
*******************************************************************************/
static unsigned long StrLength(unsigned char *Str)
{
  u32 Index = 0;

  /* Increment the Index unless the end of string */
  for(Index = 0; *Str != '\0'; Str++, Index++)
  {
  }

  return Index;
}

/*******************************************************************************
* Function Name  : ILI9325_ScrollText
* Description    :
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/
void ILI9325_ScrollText(unsigned char  Line, unsigned char *ptr)
{
  u32 i = 0, length = 0, x = 0;
  u16 refcolumn = 0;

  /* Get the string length */
  length = StrLength(ptr);

  while(1)
  {
    /* Send the string character by character on lCD */
    while ((*ptr != 0) & (i < 30))
    {
      /* Display one character on LCD */
      ILI9325_DisplayChar(refcolumn,Line, *ptr);
      /* Decrement the column position by 16 */
      refcolumn += 8;
      /* Point on the next character */
      ptr++;
      /* Increment the character counter */
      i++;
    }

    ILI9325_Delay(100);

    i = 0;
    ptr -= length;
    x++;
    if(refcolumn < 16)
    {
      x = 0;
    }

    refcolumn = 319 - (x * 16);
  }
}


/*******************************************************************************
* Function Name  : ILI9325_GetPointRGB
* Description    : Get the color data of the appointed pixel.
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short ILI9325_GetPointRGB(unsigned short Xpos,unsigned short Ypos)
{
  ILI9325_SetCursor(Xpos,Ypos);
  return(ILI9325_BGR2RGB(ILI9325_ReadRegister(R22)));
}


/*******************************************************************************
* Function Name  : ILI9325_ReadRegister
* Description    : Get the data of the appointed register.
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short ILI9325_ReadRegister(unsigned short LCD_Reg)
{
  
  //写索引寄存器
  ILI9325_WriteIndex(LCD_Reg);
  
  return(ILI9325_ReadData());
}

/*******************************************************************************
* Function Name  : ILI9325_ReadData
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short ILI9325_ReadData(void)
{
  unsigned short temp=0;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  ILI9325_CS_LOW();
  ILI9325_RS_HIGH();
  ILI9325_WR_HIGH();
  //丢掉第一次读出的不确定数
  ILI9325_RD_LOW();
  temp = GPIO_ReadInputData(GPIOE);
  ILI9325_RD_HIGH();
  
  ILI9325_RD_LOW();
  temp = GPIO_ReadInputData(GPIOE);
  ILI9325_RD_HIGH();
  
  ILI9325_CS_HIGH();
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  return temp;
}
/*******************************************************************************
* Function Name  : ILI9325_BGR2RGB
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short ILI9325_BGR2RGB(unsigned short BGR_Code)
{
  unsigned short  r, g, b;

  r = BGR_Code & 0x1f;
  g = (BGR_Code>>5)  & 0x3f;
  b = (BGR_Code>>11) & 0x1f;
  
  return( (r<<11) + (g<<5) + (b<<0) );
}

/*******************************************************************************
* Function Name  : CheckController
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
unsigned short CheckController(void)
{
  return(ILI9325_ReadRegister(R0));
}

/*******************************************************************************
* Function Name  : ILI9325_SetPixel
* Description    : 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void ILI9325_SetPixel(unsigned short Xpos,unsigned short Ypos,unsigned short RGB_Code)
{
  ILI9325_SetCursor(Xpos,Ypos);
  ILI9325_WriteGRAM(RGB_Code);
}

/******************************************************************
 - 功能描述：将一个32位的变量dat转为字符串，比如把1234转为"1234"
 - 参数说明：dat:带转的long型的变量
             str:指向字符数组的指针，转换后的字节串放在其中           
 - 返回说明：无
 ******************************************************************/

void u32tostr(unsigned long dat,unsigned char *str) 
{
 char temp[20];
 unsigned char i=0,j=0;
 i=0;
 while(dat)
 {
  temp[i]=dat%10+0x30;
  i++;
  dat/=10;
 }
 j=i;
 for(i=0;i<j;i++)
 {
  str[i]=temp[j-i-1];
 }
 if(!i) {str[i++]='0';}
 str[i]=0;
}

/******************************************************************
 - 功能描述：将一个字符串转为32位的变量，比如"1234"转为1234
 - 参数说明：str:指向待转换的字符串           
 - 返回说明：转换后的数值
 ******************************************************************/

unsigned long strtou32(unsigned char *str) 
{
 unsigned long temp=0;
 unsigned long fact=1;
 unsigned char len=StrLength(str);
 unsigned char i;
 for(i=len;i>0;i--)
 {
  temp+=((str[i-1]-0x30)*fact);
  fact*=10;
 }
 return temp;
}


//全屏显示单色
void ILI9325_FullScreen(unsigned short Color)
{
  ILI9325_Clear(Color);
}

//彩色横条纹显示测试（8条横条纹）
void ILI9325_Row_ColorBar(void)
{
  unsigned short i,j,k;
  for(k=0;k<8;k++)
  {
    for(j=0;j<40;j++)
    {
      for(i=0;i<240;i++)
      {
        switch(k)
        {
        case 0: ILI9325_WriteGRAM(White);break;
        case 1: ILI9325_WriteGRAM(Black);break;
        case 2: ILI9325_WriteGRAM(Blue);break;
        case 3: ILI9325_WriteGRAM(Red);break;
        case 4: ILI9325_WriteGRAM(Magenta);break;
        case 5: ILI9325_WriteGRAM(Green);break;
        case 6: ILI9325_WriteGRAM(Cyan);break;
        case 7: ILI9325_WriteGRAM(Yellow);break;
        default:ILI9325_Clear(White);break;
        }
      }
    }
  }
}

//彩色竖条纹显示测试(6条竖条纹）
void ILI9325_Col_ColorBar(void)
{
  unsigned short j,k;
  for(k=0;k<320;k++)
  {
    for(j=0;j<40;j++)
    {
      ILI9325_WriteGRAM(Blue);
    }
    for(j=0;j<40;j++)
    {
      ILI9325_WriteGRAM(Red);
    }
    for(j=0;j<40;j++)
    {
      ILI9325_WriteGRAM(Magenta);
    }
    for(j=0;j<40;j++)
    {
      ILI9325_WriteGRAM(Green);
    }
    for(j=0;j<40;j++)
    {
      ILI9325_WriteGRAM(Cyan);
    }
    for(j=0;j<40;j++)
    {
      ILI9325_WriteGRAM(Yellow);
    }    
  }
}

//灰阶显示测试
void ILI9325_GrayScale(void)
{
  unsigned short i,j,k;
  
  for(i=0;i<80;i++)
  {
    for(j=0;j<16;j++)
    {
      for(k=0;k<15;k++)
      {
        ILI9325_WriteGRAM((((unsigned short)(RedGrayScale[j*4]))<<8) | (RedGrayScale[j*4+1]));
      }
    }
  }
  for(i=0;i<80;i++)
  {
    for(j=0;j<16;j++)
    {
      for(k=0;k<15;k++)
      {
        ILI9325_WriteGRAM((((unsigned short)(GreenGrayScale[j*4]))<<8) | (GreenGrayScale[j*4+1]));
      }
    }
  }
  for(i=0;i<80;i++)
  {
    for(j=0;j<16;j++)
    {
      for(k=0;k<15;k++)
      {
        ILI9325_WriteGRAM((((unsigned short)(BlueGrayScale[j*4]))<<8) | (BlueGrayScale[j*4+1]));
      }
    }
  }
  for(i=0;i<80;i++)
  {
    for(j=0;j<16;j++)
    {
      for(k=0;k<15;k++)
      {
        ILI9325_WriteGRAM((((unsigned short)(WhiteGrayScale[j*4]))<<8) | (WhiteGrayScale[j*4+1]));
      }
    }
  }
}


//在制定光标处显示字符串，可自动换行
void ILI9325_DisplayString_At(unsigned short x,unsigned short y,unsigned char *pAscii,unsigned short WordColor,unsigned short BackColor)
{
  unsigned char i, j;
  unsigned char str;
  unsigned int OffSet;
  while(1)
  {
    if(x==240)//当一行显示字符超过30个字符时，下一行接着显示
    {
      x=0;
      y=y+16;
    }
          
    if (*pAscii == 0)//直到显示到字符的结尾时推出循环显示
    {
      break;
    }

    OffSet = (*pAscii - 32)*16;//寻找AsciiLib[]中相应字符的点阵数据
          /*pAscii传过来的是该字符的ASCII码数字表示（参看ASCII表，有严格
          顺序表示），减32是因为第0~31号是控制字符，第32号为“空格”字符
          除以16是因为在AsciiLib[]中的每个字符点阵为16个字节表示*/
    for (i=0;i<16;i++)//因为一个字符的高度为16
    {
      ILI9325_SetCursor(x,y+i);
      str = *(ASCII_Table + OffSet + i);  
      for (j=0;j<8;j++)//一个字符宽度为8，一个像素点为16位
      {
	 if ( str & (0x80>>j) )//0x80>>j，选择字符颜色（根据调色板）
	 {
	    ILI9325_WriteGRAM(WordColor);
	 }
	 else
	 {
	           
	    ILI9325_WriteGRAM(BackColor);
	           
	 }               				
       } 
    }
    x+=8;//传送下一个字符
    pAscii+=1;
  }
}

//显示一个汉字16*16的（需取字模）
void ILI9325_DisplayHanZi_16(unsigned short x,unsigned short y,const unsigned char *pHz,unsigned short WordColor,unsigned short BackColor)
{
  unsigned char i,j;
  unsigned int str;
  for (i=0;i<16;i++)//因为一个字符的高度为16
  {
    ILI9325_SetCursor(x,y+i);
    str = ((unsigned int)(*(pHz + i*2)<<8))|(*(pHz+i*2+1));
    for (j=0;j<16;j++)//一个汉字宽度为16，一个像素点为16位
    {
      if ( str & (0x8000>>j) )//0x80>>j，选择字符颜色（根据调色板）
      { 
        ILI9325_WriteGRAM(WordColor);
      }
      else  
      {
        
       ILI9325_WriteGRAM(BackColor);
        
      }               				
    } 
  }
}

//显示汉字串（需国标字库支持）
/*void ILI9325_Display_FONT16(unsigned short x,unsigned short y,unsigned char *pHz,unsigned short WordColor,unsigned char BackColor)
{
  unsigned char i,j;
  unsigned int str,qh,wh,offset;
  while(1)
  {
    if(x==240)//当一行显示字符超过30个字符时，下一行接着显示
    {
       x=0;
       y=y+16;
    }
          
    if (*pHz == 0)//直到显示到字符的结尾时推出循环显示
    {
       break;
    }
    
    qh = *pHz-0xa0;//区码
    wh = *(++pHz)-0xa0;//位码
    
    offset = ((94*(qh-1)+(wh-1))-1410)*32;//该显示汉字字模起始地址，减去1410，因为第一个汉字的索引为1410
  
    for (i=0;i<16;i++)//因为一个字符的高度为16
    {
      ILI9325_SetCursor(x,y+i);
      str = ((unsigned int)(*(FONT16 + offset + i*2)<<8))|(*(FONT16 + offset +i*2+1));
      for (j=0;j<16;j++)//一个汉字宽度为16，一个像素点为16位
      {
        if ( str & (0x8000>>j) )//0x80>>j，选择字符颜色（根据调色板）
        { 
          ILI9325_WriteGRAM(WordColor);
        }
        else  
        {
          ILI9325_WriteGRAM(BackColor);
        }               				
      } 
    }
    x+=16;
    pHz++;//显示下一个汉字
  }
}*/

//显示一个汉字48*48的（需取字模）
void ILI9325_DisplayHanZi_48(unsigned short x,unsigned short y,const unsigned char *pHz,unsigned short WordColor,unsigned short BackColor)
{
  unsigned char i,j,k;
  unsigned int str;
  for (i=0;i<48;i++)//因为一个字符的高度为48
  {
    ILI9325_SetCursor(x,y+i);
    for(k=0;k<6;k=k+2)//48=3*16
    {
      str = ((unsigned int)(*(pHz + i*6+k)<<8))|(*(pHz+i*6+1+k));
      for (j=0;j<16;j++) 
      {
        if ( str & (0x8000>>j) )//0x80>>j，选择字符颜色（根据调色板）
        { 
          ILI9325_WriteGRAM(WordColor);
        }
         else  
        {
        
          ILI9325_WriteGRAM(BackColor);
        
        }               				
      } 
    }
  }
}

//显示指定大小的图片（图片宽和高最好为偶数）
void ILI9325_DrawBMP(unsigned short x,unsigned short y,unsigned short Width,unsigned short Height,const unsigned char *pBMP)
{
  unsigned short k=0,m=1;
  unsigned short i,j;
  
  ILI9325_SetCursor(x,y);
 
  for(i=0;i<Height;i++)  //行坐标
  {
     for(j=0;j<Width;j++) //列坐标
     {
       ILI9325_WriteGRAM((((unsigned short)(pBMP[k]))<<8) | pBMP[m]);
       k=k+2;
       m=m+2;
     }
  } 
}







