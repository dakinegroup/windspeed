/* File: usart_int.c
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
/*
File description:
  Provides for USART Interrupt handling for transmitter and receiver
  Following user interface functions are avialable:
  a. USART_Init -- initialize interrupts, baud
  b. USART_Transmit_String -- schedules a string for serial transmission
  c. USART_Transmit -- serially transmits only one byte/char
  d. USART_Receive -- checks if there is anything received from serial
  e. USART_Transmit_String_P -- transmits strings from program memory
  f. USART_Transmit_Bytes_P -- transmits data in bytes from program memory
  g. USART_Receive_String -- receives complete line of tex
  h. USART_Receive_String -- incomplete, but intends to retrieve machine to 
     machine talk..
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include "usart.h"

/* Remember there is only 2k of RAM */

struct{
    unsigned char buffer[BUFFER_SIZE];
    unsigned char last_write;
    unsigned char last_read;
    char nearly_full;
    char empty;
} txQueue;

struct {
    unsigned char buffer[RX_BUFFER_SIZE];
    unsigned char last_write;
    unsigned char last_read;
    char userInputStringReceived;
} rxQueue;

#if STATS_ENABLED
struct  {
    int logs_truncated;
    int logs_dropped;
    int logs_delivered;
    int logs_silence_events;
} usartStats;
#endif

void initRxQueue() {
    int i;
    for(i=0; i < RX_BUFFER_SIZE; i++) {
            rxQueue.buffer[i] = 0;
        }   
    rxQueue.last_write = rxQueue.last_read = 0;
    rxQueue.userInputStringReceived = 0;
}

void initTxQueue() {
    int i;
    txQueue.last_write=0;
    txQueue.last_read=0;
    txQueue.nearly_full=0;
    txQueue.empty=1;
    for(i=0; i < BUFFER_SIZE; i++) {
        txQueue.buffer[i] = 0;
    }   
}
void USART_Init( unsigned int ubrr)
{
    /*Set baud rate */
    UBRRH = (unsigned char)(ubrr>>8);
    UBRRL = (unsigned char)ubrr;
    /*Enable receiver and transmitter */
    UCSRB = UCSRB | (1<<TXEN) | (1<<RXEN) ;
    UCSRB = UCSRB |  (1 << TXCIE) | (1 << RXCIE) ;
    // enable receiver interrupt
    //| (1 << RXCIE0) ;
    // not sure how does it help, currently using only tx complete interrupt, it suffices
    //|(1 << UDRIE0);

    // Initialize tx and rx buffer
    initTxQueue();
    initRxQueue();
}

void USART_Transmit(unsigned char data) {
/* Wait for empty transmit buffer */
while ( !( UCSRA & (1<<UDRE)) )
;
  
/* Put data into buffer, sends the data */
UDR = data;
}

/*
  Developer Notes:
  this is done in initial stages, just preserved here. Better way to do
  is to use sprintf, itoa functions. One need to see, which one generates
  compact code here.
*/
void printByte(unsigned char p) {
unsigned char c;
    c = (p >> 4) & 0x0f;
    if(c < 10) {
        USART_Transmit('0' + c);    
    } else {    
    USART_Transmit(c+'A'-10);           
        //USART_Transmit(getCharacter(c));      
    }
    c = p  & 0x0f;
    if(c < 10) {
        USART_Transmit('0' + c);    
    } else {
        USART_Transmit(c+'A'-10);       
        //USART_Transmit(getCharacter(c));      
    }
}


/*
  Developer Notes:
  This was useful to read the contents from program memory, specifically strings
  It need to be enhanced such that constant strings are saved in program memory and
  variable data is picked from RAM. This will help conserve RAM while using debug
  messages. Also, we need to create constants file for message strings as well.
*/
void USART_Transmit_String_P(const char *x) {
int i = 0;
char c;
    for(i=0; i < 100; i++) {
            //printByte(pgm_read_byte((char *)(0x0000 + i)));
        c = pgm_read_byte(&x[i]);
        if(c)
            USART_Transmit(c);
        else break;
    }
    USART_Transmit('\r');
    USART_Transmit('\n');
}

/*
  Developer Notes:
  This is done to do the dump of program memory for debugging purposes.
  Future enhancements: Get the argument as address and length of the bytes to be
  displayed. While doing this, one need to keep in mind, the tx buffer. Because
  if this information is dumped continously, it will be truncated. Therefore, we
  need to provide a user intervention asking for "more.."
*/
void USART_Transmit_Bytes_P() {
int i = 0;
char c;
    return;
    for(i=0; i < 100; i++) {
            //printByte(pgm_read_byte((char *)(0x0000 + i)));
        c = pgm_read_byte((char *)(0x05BA + i));
        printByte(c);
        USART_Transmit(' ');
    }
    USART_Transmit('\r');
    USART_Transmit('\n');
}

/* Developer Notes:
   This is a blocking call. It waits continously for transmission end and transfers next character
   This is very slow and may be useful, where there is no backend work being done as part of main loop.
   Interrupt based work is anyway happening.
*/
void USART_Transmit_String(char *x) {
int i = 0;
char c;
    for(i=0; i < 100; i++) {
        c = x[i];
        if(c)
            USART_Transmit(c);
        else break;
    }
    USART_Transmit('\r');
    USART_Transmit('\n');
}

/* Developer Notes:
   PING PONG Buffer being maintained.
   Further it can be enhanced to implement circular buffer with prevention for overwriting.
*/
ISR(USART_TXC_vect)
{
  cli();
  if(txQueue.last_read < txQueue.last_write) {
      UDR = txQueue.buffer[txQueue.last_read];
      txQueue.last_read=txQueue.last_read+1;    
  } else {
    txQueue.last_read = 0;
    txQueue.last_write= 0;
  }            
  sei();
}
//ISR(USART_UDRE_vect, ISR_ALIASOF(USART_TX_vect));
/* Developer Notes:
   Though it is written here, not much use is envisaged currently, till we have machine to
   machine communications and a independent interrupt on receipt of a frame. Need to look
   at such software interrupt.
*/
ISR(USART_RXC_vect)
{
  char tchar;
  cli();

  if(rxQueue.last_write >= RX_BUFFER_SIZE ||
      rxQueue.userInputStringReceived == 1) {
     tchar = UDR; //drop this byte;
     UDR = tchar;
  } else {
    rxQueue.buffer[rxQueue.last_write] = UDR;
     UDR = rxQueue.buffer[rxQueue.last_write];
     if(!rxQueue.buffer[rxQueue.last_write] 
      || rxQueue.buffer[rxQueue.last_write] == '\r' 
      || rxQueue.buffer[rxQueue.last_write] == '\n') {
            rxQueue.buffer[rxQueue.last_write] = 0;
              rxQueue.userInputStringReceived = 1;
            
        }

    rxQueue.last_write++;
  }
  sei();
}

/* Developer Notes:
   This is non-blocking call. It schedules data transfer - irrespective of string or raw data
   In case transmission has gone silent, it detects and wakes up the transceiver for rest of
   the transmission to happen using ISR
*/
void USART_Transmit_String2(char *x) {
    int i=0;
    #if STATS_ENABLED
    int trunc = 0;
    #endif
    cli();
    if(txQueue.last_write == 0) {
        UDR = '$';
        #if STATS_ENABLED
        usartStats.logs_silence_events++;
        #endif
        for(i=0; i < BUFFER_SIZE/2 && x[i] != 0; i++) {
            txQueue.buffer[i] = x[i];
            if((txQueue.last_write + i) >= (BUFFER_SIZE-1)) {
                #if STATS_ENABLED
                  trunc = 1;
                #endif
                break;
            }
        }
        txQueue.last_write = i;
        #if STATS_ENABLED
        if(trunc) {
            usartStats.logs_truncated++;
        } else {
            usartStats.logs_delivered++;
        }
        #endif
    } else if(txQueue.last_write > 0 
            && txQueue.last_write < BUFFER_SIZE) {
        for(i=0; i < BUFFER_SIZE/2 && x[i] != 0; i++) {
            txQueue.buffer[i+txQueue.last_write] = x[i];
            if((txQueue.last_write + i) >= (BUFFER_SIZE-1)) {
                #if STATS_ENABLED
                  trunc = 1;
                #endif
                break;
            }
        }
        #if STATS_ENABLED
        if(trunc) {
            usartStats.logs_truncated++;
        } else {
            usartStats.logs_delivered++;
        }
        #endif
        txQueue.last_write = i + txQueue.last_write;
    } else {
        #if STATS_ENABLED
        usartStats.logs_dropped++;
        #endif
    }
    sei();
}
#if STATS_ENABLED
void USART_PrintStats() {
    static char logMsg [100];
    sprintf(logMsg, "\r\nDelivered: %d\r\nTruncated: %d\r\nDropped: %d\r\nSilence Events: %d\r\n",
                 usartStats.logs_delivered,
                 usartStats.logs_truncated,
                  usartStats.logs_dropped,
                  usartStats.logs_silence_events);
    USART_Transmit_String2(logMsg);
}
#endif


/* Developer Notes:
   This is blocking call, we receive data / command from user.
   Further enhancements - we can make it non-blocking and take a
    callback for processing commands.
*/
unsigned char* USART_Receive_String() {
    static unsigned char inputs[50];
    unsigned char i=0;;
    while(1) {
        if(i >= 49) {
            inputs[49] = 0;
            break;
        }
        while ( !( UCSRA & (1<<RXC)) )
;
//loop_until_bit_is_set(UCSR0A, );
        inputs[i] = UDR;
        USART_Transmit(inputs[i]);
        if(!inputs[i] || inputs[i] == '\r' || inputs[i] == '\n') {
            inputs[i] = 0;
            break;
        }
        i++;
    }
    USART_Transmit_String("\r\n You have typed: ");
    //Issue: Here use of strcat("You have typed", inputs) had caused crash
    USART_Transmit_String((char *)inputs);
    USART_Transmit_String("\r\n");
    return inputs;
}

/* Developer Notes:
   Not much utility is understood here for user interface purposes
   However, if there is a machine to machine communication, where we
   don't have requirement to display what is coming in, it may be useful
   to create receive routine here and identify frames, which are saved 
   in independent buffers 
*/
   
int USART_Receive_String2(char *buff, int len) {
    int i = 0;
    cli();
    if(rxQueue.userInputStringReceived) {
      for(i=0; i < rxQueue.last_write && i < len; i++) {
        buff[i] = rxQueue.buffer[i];
      }
      rxQueue.last_write =0; rxQueue.last_read = 0;
      rxQueue.userInputStringReceived = 0; /* ready to receive next command */
    }
    sei();
  return i;
}
