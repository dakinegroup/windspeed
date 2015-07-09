 #include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "stdlib.h"
#include "usart.h"
#include "mytimer.h"
#include "lcd.h"
/*
  Green on the cable is 1
  On board, where first green is seen that side is towards pin 1 of connector

|Connector | 328 Pins  |  595 pins  | LCD Pins|
|----------|-----------|------------|---------|
|1 (Green) |  1/RESET  |  10/MR#    |    x    |
|----------|-----------|------------|---------|
|     2    |    5/PD3  |   11/SHCP  |    x    |
|----------|-----------|------------|---------|
|     3    |     6/PD4 |   12/STCP  |         |
|----------|-----------|------------|---------|
|     4    |    4/PD2  |   14/DS    |         |
|----------|-----------|------------|---------|
|     5    |    11/PD5 |      x     |   E     |
|----------|-----------|------------|---------|
|6 (Black) |  13/PD7   |      x     |         |
|----------|-----------|------------|---------|
|     x    |    x      |      0/Q0  |    x    | ignored in byte formation
|----------|-----------|------------|---------|
|     x    |    x      |      1/Q1  |   D4    |
|----------|-----------|------------|---------|
|     x    |    x      |      2/Q2  |   D5    |
|----------|-----------|------------|---------|
|     x    |    x      |      3/Q3  |   D6    |
|----------|-----------|------------|---------|
|     x    |    x      |      4/Q4  |   D7    |
|----------|-----------|------------|---------|
|     x    |    x      |      5/Q5  |  RS     |   
|----------|-----------|------------|---------|
|     x    |    x      |      x     |RW to GND|     
|----------|-----------|------------|---------|
|     x    |    x      |  OE#to GND |    x    |
|----------|-----------|------------|---------|

To give command to LCD
     BITs --->
Data at 595:    x x x 0/RS Q4 Q3 Q2 Q1
Data at PD5:    00 - 0F (command)
Data at PD3:    clock to put it in input shift
Data at PD4:    high and low, to transfer out
Data at PD6:    high and low, to transfer to LCD

To give data to LCD
Data at 595:    x x x 1/RS Q4 Q3 Q2 Q1
Data at PD5:    10 - 1F (data)
Data at PD3:    same as above
Data at PD4:    same as above
Data at PD6:    same as above

To give data to shift register
*/

#define SR_SHCP 2 /* on PC2 */
#define SR_STCP 4
#define SR_DS 1 /* on PC1 */
#define LCD_EN 5

#define SINGLE_STEP_BIT_LCD 0
#define SINGLE_STEP_LCD 0
void initLCD() {
    int i;
  DDRD = DDRD | _BV(4) |_BV(5) ;
  DDRC = DDRC | _BV(2) |_BV(1) ;
  sendDataToLCD(0x03,LCD_DELAY);
  USART_Transmit_String("First init");
  sendDataToLCD(0x03,LCD_DELAY);
  USART_Transmit_String("Second init");
  sendDataToLCD(0x03,LCD_DELAY);
  USART_Transmit_String("Third init");
  for(i=0; i < 1000; i++) {
    asm("nop");
  }

  /* Set 4-bit interface */
  sendDataToLCD(0x02,LCD_DELAY);
   USART_Transmit_String("4-bit");
 /* send command to setup font etc */
  sendDataToLCD(0x02,0);
  sendDataToLCD(0x0C,LCD_DELAY);
  /* off display */
    USART_Transmit_String("Off display");
    sendDataToLCD(0x00,0);
    sendDataToLCD(0x08,LCD_DELAY);
  /* clear display */
    sendDataToLCD(0x00,0);
    sendDataToLCD(0x01,LCD_DELAY_CLEAR);
 /* entry mode set */
    sendDataToLCD(0x00,0);
    sendDataToLCD(0x06,LCD_DELAY);
/* on display */
    sendDataToLCD(0x00,0);
    sendDataToLCD(0x0D,LCD_DELAY);
  USART_Transmit_String("LCD Cmd Over");

/* Send cursor to home */
   sendDataToLCD(0x00,0);
   sendDataToLCD(0x01,LCD_DELAY);  
}


void sendShClockEdge(){
    char cmds[10];
//  PORTD = PORTD & (~ _BV(SR_SHCP));
  PORTC = PORTC & (~ _BV(SR_SHCP));
  #if SINGLE_STEP_BIT_LCD  
  USART_Transmit_String("Sh-Low");  
    while(USART_Receive_String(cmds, 30) <=0)
        asm("nop");
  #endif

//  PORTD = PORTD | _BV(SR_SHCP);
  PORTC = PORTC | _BV(SR_SHCP);
  #if SINGLE_STEP_BIT_LCD  
  USART_Transmit_String("Sh-High");  
    while(USART_Receive_String(cmds, 30) <=0)
        asm("nop");
  #endif
//  PORTD = PORTD & (~ _BV(SR_SHCP));
  PORTC = PORTC & (~ _BV(SR_SHCP));
  #if SINGLE_STEP_BIT_LCD  
  USART_Transmit_String("Sh-Low");  
    while(USART_Receive_String(cmds, 30) <=0)
        asm("nop");
  #endif
    }
void sendStClockEdge(){
    char cmds[10];
  PORTD = PORTD & (~ _BV(SR_STCP));
  #if SINGLE_STEP_BIT_LCD  
  USART_Transmit_String("St-Low");
    while(USART_Receive_String(cmds, 30) <=0)
        asm("nop");
  #endif
  PORTD = PORTD | _BV(SR_STCP);
  #if SINGLE_STEP_BIT_LCD  
    while(USART_Receive_String(cmds, 30) <=0)
        asm("nop");
  USART_Transmit_String("St-High");
  #endif
  PORTD = PORTD & (~ _BV(SR_STCP));
  #if SINGLE_STEP_BIT_LCD    
  USART_Transmit_String("St-Low");
    while(USART_Receive_String(cmds, 30) <=0)
        asm("nop");
  #endif
}
void enableLCD(int delay) {
int i;
  PORTD = PORTD | _BV(LCD_EN);
 //USART_Transmit_String("Check LCD Enable - High");
  //wait(1000);/* is it required??*/
  PORTD = PORTD & (~ _BV(LCD_EN));
 //USART_Transmit_String("Check LCD Enable - LOW");
  wait(1); /*is it required, may be to give time for data to 
    latch at rising edge??*/
}
void sendDataToLCD(unsigned char data, int delay) {
    unsigned int i, bit;
    char cmds[10];
  #if SINGLE_STEP_LCD
    sprintf(cmds, "%02x\r\n", data);
    USART_Transmit_String(cmds);
  #endif
    data = data << 1;
    for(i=0;i<8;i++) { 
    /* though only 4bits are relevant, lets do it all */
      /* extract bit - highest bit first on shift, 
      it goes to MSB, after 8 shifts */
      bit = data & (1 << (7-i));

      if(bit) {
//        PORTD = PORTD | _BV(SR_DS);
        PORTC = PORTC | _BV(SR_DS);
      #if SINGLE_STEP_BIT_LCD
        USART_Transmit_String("1");
        #endif
      }
      else {
        #if SINGLE_STEP_BIT_LCD
        USART_Transmit_String("0");
      #endif
//        PORTD = PORTD & (~ _BV(SR_DS));    
        PORTC = PORTC & (~ _BV(SR_DS));    
      }
      /* time to clock it in */
      sendShClockEdge();
     #if SINGLE_STEP_LCD    
    while(USART_Receive_String2(cmds, 30) <=0)
        asm("nop");
    #endif
    }
    
    /* now move out data to output register */
    sendStClockEdge();
    /* now let it be registered by LCD */
    enableLCD(delay);
 //   USART_Transmit_String2(cmds);
}

/*
   Developer Notes:
   r - takes values 0, 1
   c - takes values 0 - 40
*/
void LCD_gotoXY(int r, int c) {
  int i;
  unsigned char ddram_address;
  //USART_Transmit_String("LCD Goto");
  /* Goto to home first */
   sendDataToLCD(0x00 , 0);
   sendDataToLCD(0x02 , 0); 
/* Goto 30 character */
  /*for(i=0; i < (r*40+c); i++) {
    sendDataToLCD(0x01,0);
    sendDataToLCD(0x04,0);        
  }*/
    ddram_address = (r*40+c) & 0x7F;
    sendDataToLCD(0x08 | ((ddram_address & 0xF0)>>4), 0);
    sendDataToLCD(0x00 | (ddram_address & 0x0F), 0);
}

void LCD_WriteXY(int r, int c, char *str) {
    unsigned char ddram_address;
     int i=0;
    sendDataToLCD(0x00 , 0);
    sendDataToLCD(0x02 , 0);
    ddram_address = (r*40+c) & 0x7F;
    sendDataToLCD(0x08 | ((ddram_address & 0xF0)>>4), 0);
    sendDataToLCD(0x00 | (ddram_address & 0x0F), 0);
    LCD_Write(str);
}

void LCD_GotoHome() {
   sendDataToLCD(0x00 , 0);
   sendDataToLCD(0x02 , 0); 
}
/* scroll screen to left or right */
void LCD_SetScreenScroll(int left_right) {
 /* entry mode set */
    if(left_right == 0) {
        sendDataToLCD(0x00,0);
        sendDataToLCD(0x07,LCD_DELAY);
    } else {
        sendDataToLCD(0x00,0);
        sendDataToLCD(0x05,LCD_DELAY);
    }
}
void LCD_StartLeftScroll() {
    sendDataToLCD(0x00,0);
    sendDataToLCD(0x05,LCD_DELAY);
}
void LCD_Clear() {
  /* clear display */
    sendDataToLCD(0x00,0);
    sendDataToLCD(0x01,LCD_DELAY_CLEAR);    
}
void LCD_Writen(char *str, int len) {
int i;
  for(i = 0; i < len; i++) {
   sendDataToLCD(0x10 | ((str[i] & 0xF0)>>4),0);
   sendDataToLCD(0x10 | (str[i] & 0x0F),0);
  }
}

void LCD_Write(char *str) {
int i;
  for(i = 0; str[i] !=0; i++) {
   if(i > 15) { 
   // if display is going out of visibility, shift left
    sendDataToLCD(0x01,0);
    sendDataToLCD(0x08,0);
   }
   sendDataToLCD(0x10 | ((str[i] & 0xF0)>>4),0);
   sendDataToLCD(0x10 | (str[i] & 0x0F),0);
  }
}

