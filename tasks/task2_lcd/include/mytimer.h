#ifndef __MYTIMER_H__
#define __MYTIMER_H__

extern unsigned int timestamp[];
extern unsigned char debug_led;
typedef int (* tTimedCallBack)(int);
void initTimedTasks();
int resetAllTimers(int);
void repeat(int, tTimedCallBack);
void invokeScheduledItem();
void doOnce( int ms, tTimedCallBack);
void wait(int ms);
#define USE_DEBUG_LED 1
void initTimer();
#endif /* __MYTIMER_H__ */
