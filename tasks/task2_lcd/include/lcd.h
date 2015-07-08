#ifndef __LCD_H__
#define __LCD_H__

#define LCD_DELAY 1
#define LCD_DELAY_CLEAR 2
void initLCD();
void sendDataToLCD(unsigned char data, int delay);
void LCD_gotoXY(int r, int c) ;
void LCD_Clear();
void LCD_Writen(char *str, int len);
void LCD_Write(char *str);
void LCD_WriteXY(int r, int c, char *str);
#endif /* __LCD_H__ */