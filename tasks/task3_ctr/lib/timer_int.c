/* File: timer_int.c
 * Copyright (C) da Kine Technologies (P) Ltd. - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Vineet Maheshwari <vineet@dakinegroup.com>, June 2015
 * Revision History
 * Date        Ticket   Change Note
 * -----------------------------------
 * 20-Jun-2015  N/A     First Version Ready
 * 
 */

 #include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "mytimer.h"


unsigned int timestamp[2] = {0,0};
unsigned char debug_led = 1;

void initTimer() {
  sei();
  TIMSK = TIMSK | _BV(TOIE1);
  TCCR1B = (TCCR1B & 0xF8) | 0x01;
  #if USE_DEBUG_LED
  DDRB = DDRB | _BV(DDB1);
  PORTB = PORTB & ~(_BV(1));
  debug_led = 0;
  #endif
}
ISR(TIMER1_OVF_vect)
{
  cli();
 //     PORTB = PORTB | _BV(1);
  timestamp[0] ++;
  if(timestamp[0] == 0) {
    timestamp[1] ++;
  }
  #if USE_DEBUG_LED
  if((timestamp[0] & 0x003F) == 0) {
    if(debug_led == 0) {
      debug_led = 0x01;
      PORTB = PORTB | _BV(1);
    } else {
      debug_led = 0;
      PORTB = PORTB & ~(_BV(1));
    }
  }
  #endif
  sei();
}