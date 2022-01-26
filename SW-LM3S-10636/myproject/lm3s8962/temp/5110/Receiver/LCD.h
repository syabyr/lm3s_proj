extern void lcd_sendbyte(unsigned char dat,unsigned char command);
extern void lcd_set_xy(unsigned char x,unsigned char y);
extern void lcd_clear();
extern void lcd_init();
extern void lcd_write_cn(unsigned char x,unsigned char y,unsigned char c);
extern void lcd_write_cns(unsigned char x,unsigned char y,unsigned char s,unsigned char num,unsigned lrow);
extern void lcd_write_char(unsigned char c);
extern void lcd_write_stringxy(unsigned char x,unsigned char y,unsigned char *p);
extern void lcd_write_string(unsigned char *p);
extern void LCD_Disp_Num(unsigned char x,unsigned char y,unsigned int Num);
extern void LCD_Disp_BNum(unsigned char x,unsigned char y,unsigned char Num);


extern void LCD_Disp_clkdot();
extern void LCD_Disp_clkdot_blank();

extern void LCD_Disp_TI();




