#include <p33FJ128MC802.h>
#include "timer.h"
#include "main.h"

#define PIN_TIMER 5						// PORT B PIN 5
#define PIN_ERR 6						// PORT B PIN 6

volatile unsigned char new_frame=0,timer_clash=0;

void InitTimer()
{
	T1CONbits.TON = 0;					// Disable Timer
	T1CONbits.TCS = 0;					// Select internal instruction cycle clock 
	T1CONbits.TGATE = 0;				// Disable Gated Timer mode
	T1CONbits.TCKPS= 0b11;				// Internal clock prescaled at 256 => 40 MHz
	TMR1 = 0x00; 						// Clear timer register
	PR1 = 2866;        					// Load the period value (3125 * 50/46) + 
										// one more time correction based on oscilloscope
	IPC0bits.T1IP = 0x01;				// Set Timer1 Interrupt Priority Level
	IFS0bits.T1IF = 0;					// Clear Timer1 Interrupt Flag
	IEC0bits.T1IE = 1;					// Enable Timer1 interrupt
	T1CONbits.TON = 1;					// Start Timer
	TRISB=TRISB & ~((1<<PIN_TIMER)|(1<<PIN_ERR));
}

/* Example code for Timer1 ISR */
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
	/* Interrupt Service Routine code goes here */
#ifdef TIMER_DEBUG
	LATB=PORTB ^ (1<<PIN_TIMER);		// Toggle @ Fs/2
	if(still_working==1)
	{
		LATB=PORTB | (1<<PIN_ERR);		// Frame correct
	}	
	else
	{
		LATB=PORTB & ~(1<<PIN_ERR);		// Frame wrong
	}
#endif
	timer_clash=still_working;			// If still_working then timer_clash :)
	new_frame=1;
	IFS0bits.T1IF = 0; 					// Clear Timer1 Interrupt Flag
}	
