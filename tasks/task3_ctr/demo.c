/* File: tc_main.c
 * Copyright (C) da Kine Technologies (P) Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Vineet Maheshwari <vineet@dakinegroup.com>, June 2015
 * Revision History
 * Date        Ticket   Change Note
 * -----------------------------------
 * 
 */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "iocompat.h" /* Note [1] */
#include "usart.h"
#include "mytimer.h"

#define FOSC 16000000UL // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

enum { UP, DOWN };


ISR(BADISR_vect)
{
    // user code here
}

void ioinit() {

USART_Init(51); 
/* IMPORTANT: 41 values works with -mmcu=avr5, where it should have been */
/* IMPORTANT: 51 works with -mmcu=atmega328p, strange.. what is the secret here???*/

//Below code may be removed, i believe this was for PWM
//TCCR1B = (TCCR1B & 0xF8) | 0x01;
//TIMSK1 = TIMSK1 | _BV(TOIE1);

initTimer();
USART_Transmit_String("Restarted: "); 
initLCD();
sei ();
}

#define HIGH 1
#define LOW 0

int
main (void)
{
  int i;
  char testIntStr[20];

  ioinit ();
//USART_Transmit_String2("This is a test..");

USART_Transmit_String("How are you???");
itoa(0x23,testIntStr,16);
USART_Transmit_String2(testIntStr);
USART_Transmit_String2("Start 1");
USART_Transmit_String2("Start 2");
LCD_gotoXY(0,0);
LCD_Write("Restarting...");
LCD_gotoXY(0,0);
LCD_Clear();
wait(245*1);
LCD_Write("  Welcome to");
LCD_gotoXY(1,0);
LCD_Write("   Amity-46");
wait(245*5);
LCD_Clear();
LCD_gotoXY(0,0);
LCD_Write("Wind(Speed,Dir)");
//USART_Receive_String();
//USART_Transmit_String2("This is what is typed in ..");
//USART_Transmit_String2(USART_Receive_String());
#if STATS_ENABLED
USART_PrintStats();
#endif
//USART_Transmit_String2("Very well, thank you8???");
initRotations();
initDirectionSensors();
/* this probably is taken care of by gcc main program exit */
 while(1) {
    wait(245/2);
     LCD_gotoXY(1,0);
     sprintf(testIntStr,"%03dr/s, ", readRotations());
     resetRotations();
     LCD_Write(testIntStr);
     USART_Transmit_String(testIntStr);
     LCD_Write(readDirection());
     USART_Transmit_String(readDirection());
    
    asm("nop");
  }
  return (0);
}
