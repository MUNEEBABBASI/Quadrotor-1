#ifndef _main_h
#define _main_h

#define xMAIN_CYCLES					// Count the time left with the main

#define PACK_TIMEOUT 200000				//120 ms wait
										// .6 uS => 12 ms. => Total 24 ms wait

#ifdef MAIN_CYCLES
extern unsigned char flag1c,flag2c;
#endif

#endif
