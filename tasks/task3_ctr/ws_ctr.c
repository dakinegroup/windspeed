/* File: ws_cntr.c
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
#include "ws_ctr.h"

int rotations=0;

enum { UP, DOWN };
/*ISR(BADISR_vect) {
}
*/
/*ISR(INT0_vect) {
  cli();
   rotations++;
  sei();
}
*/
unsigned char pinb_history=0;

ISR(INT1_vect) {
  cli();
  sei();
}

ISR(INT0_vect) {
//unsigned char chgBits=0;
  cli();
  rotations++;
  /*chgBits = pinb_history ^ PIND2;
  if(chgBits) {
    rotations++;
  }
  pinb_history = PIND2;*/
  sei();
}

/* INT0 configured for sensing rotations */

void initRotations() {
     DDRD  &= ~(1 << PD2);
     PORTD |= (1 << PD2);
     MCUCR |= (1 << ISC01);
     GICR = GICR | _BV(INT0);
     sei ();
}

/* PC3,4,5 are configured as input for 8 sensors 3 bit encoded
   data
   */
void initDirectionSensors() {
    /* set PORTD for input*/
    DDRC  &= ~(1 << PC3);
    PORTC |= (1 << PC3);

    DDRC  &= ~(1 << PC4);
    PORTC |= (1 << PC4);

    DDRC  &= ~(1 << PC4);
    PORTC |= (1 << PC4);
}

#define HIGH 1
#define LOW 0

int readRotations() {
     int counter;
          cli();
          counter = rotations;
          sei();
        
    return counter;

}
void resetRotations(){
    cli();
    rotations = 0;
    sei();

}

char* directions[9] = {"North ", "N-East", "East  ", "S-East", "South ", "S-West", "West  ", "N-West", "Invali"};
/*char* readDirectionByte() {
  unsigned char direction = 0x00;
  char ab[16];
  direction = PINC & ((1 << PINC3) | (1 << PINC4) | (1 << PINC5));
  direction = direction >> 3;
  sprintf(ab, "%02x", direction);
  //USART_Transmit_String(ab);
  return ab;
}*/
char* readDirection() {
  unsigned char direction = 0x00;
  char ab[16];
  direction = PINC & ((1 << PINC3) | (1 << PINC4) | (1 << PINC5));
  direction = direction >> 3;
  sprintf(ab, "DirByte: %02x", direction);
  USART_Transmit_String(ab);
  return directions[direction];
}
