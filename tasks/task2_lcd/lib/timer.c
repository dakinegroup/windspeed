/* File: timer.c
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
#include <stdlib.h>
#include "mytimer.h"
 #include "usart.h"
#define MAX_TASKS 10
struct {
    int empty;
    int timestamp[2];
    tTimedCallBack cb;
    int recurrence;
} scheduledItems[MAX_TASKS];



void initTimedTasks() {
    int i = 0;
    for (i=0; i < MAX_TASKS; i++) {
        scheduledItems[i].empty = 1;
        scheduledItems[i].cb = 0;
        scheduledItems[i].timestamp[0] = 0;
        scheduledItems[i].timestamp[1] = 0;
    }
}

int resetAllTimers(int x)  {
    int i;
    cli();
    timestamp[0] = 0;
    timestamp[1] = 0;
    for(i=0; i < MAX_TASKS; i++) {
        scheduledItems[i].timestamp[0] = 0;
        scheduledItems[i].timestamp[1] = 0;
    }
    sei();
}

void repeat(int ms, tTimedCallBack cb) {
int i = 0;

    //add scheduled item to the queue
    for (i = 0; i < MAX_TASKS; i++) {
        if(scheduledItems[i].empty) {
            cli();
            scheduledItems[i].timestamp[0] = timestamp[0];
            scheduledItems[i].timestamp[1] = timestamp[1];
            sei();
            scheduledItems[i].timestamp[0] += ms;
            scheduledItems[i].recurrence = ms;
            scheduledItems[i].cb = cb;
            
            scheduledItems[i].empty = 0;
            break;
        }
    }
}

void invokeScheduledItem() {
    int ts[2], i;
    char bytes[30];
    cli();
            ts[0] = timestamp[0];
            ts[1] = timestamp[1];
    sei();
    
    //retrieve scheduled item from queue and mark it free for reuse
    for (i = 0; i < MAX_TASKS; i++) {
        if(!scheduledItems[i].empty) {
            if(ts[0] > scheduledItems[i].timestamp[0]) {
                /*sprintf(bytes,"%02x%02x: Thr: %02d, CB: %04x\r\n", timestamp[1], timestamp[0], i, scheduledItems[i].cb);
                USART_Transmit_String2(bytes);*/
             if(scheduledItems[i].cb != 0) {
                (*scheduledItems[i].cb)(0);
            }
            scheduledItems[i].timestamp[0] += scheduledItems[i].recurrence;
            }                           
        }
    }
}

void doOnce( int ms, tTimedCallBack cb) {
    // to schedule a callback to be invoked after given seconds from now
}

void wait(int ms) {
unsigned int ts[2], t[2];
    cli();
            ts[0] = timestamp[0];
            ts[1] = timestamp[1];
    sei();
    if((0xFFFF - ts[0]) < ms) {
        ts[1] += 1;
    } 
    ts[0] += ms;
 while(1) {
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    cli();
            t[0] = timestamp[0];
            t[1] = timestamp[1];
    sei();
    if(ts[1] > t[1]) {
    } else if(ts[1] == t[1]) {
        if(ts[0] > t[0]) {

        } else {
            break; //time up
        }
    } else if(ts[1] < t[1]) {
        break; //time up
    }
 }    
}