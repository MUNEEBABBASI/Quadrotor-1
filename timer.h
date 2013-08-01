#ifndef _timer_h
#define _timer_h

#define xTIMER_DEBUG					// Count the time left with the main

volatile extern unsigned char new_frame,timer_clash;

void InitTimer(void);

#endif
