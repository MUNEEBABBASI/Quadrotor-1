#include <p33FJ128MC802.h>
#include "adc.h"
#include "muart.h"

#define ADC_DONE (1<<0)
#define ADC_SAMP (1<<1)
//#define ADON (1<<15)

unsigned int z=0;
float zfifo[]={0,0,0},zfilt[]={0,0};
unsigned char zH=0,zL=0,t=0;
float fA[]={-1.6169,0.6794},fB[]={0.0164,0.0296,0.0164};
// ADC pin is pin 7 of the uC.

void InitADC()
{
//	TRISB = TRISB | (1<<3);           // make sure ADC pins are set to be input
//	//AD1PCFGL = (1<<5);	//PCFG5
//	//AD1CON3 =???
//	
//	//AD1CON2 = VCFG0;	// By default Uses AVdd and AVss as reference, Single channel conversion
//	//AD1CON1 = SSRC;	// By default Clearing Sample bit ends sampling and starts conversion, 
//						// 10 bit operation resulting in integer
//
//	AD1CON1 = AD1CON1 | ADON;
	AD1CON1bits.FORM = 0;			// Data Output Format: 10 bit integer
	AD1CON1bits.SSRC = 0;			// Sample when sample bit is cleared
	AD1CON1bits.ASAM = 0;			// ADC Sample Control: Sampling begins after SAMP bit is set
	AD1CON1bits.AD12B = 0;			// 10-bit ADC operation

	AD1CON2bits.CHPS = 0;			// Converts CH0
    
	AD1CON3bits.ADRC = 0;			// ADC Clock is derived from Systems Clock
	AD1CON3bits.ADCS = 63;			// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*64 = 1.6us 
									// ADC Conversion Time for 10-bit Tc=12*Tad = 19.2us 
									// Maximum sampling frequency 52 kHz
				
	AD1CON1bits.ADDMABM = 0; 		// No DMA buffers
	AD1CON2bits.SMPI = 0;			// SMPI must be 0

    //AD1CHS0: A/D Input Select Register
    AD1CHS0bits.CH0SA = 5;			// MUXA +ve input selection (AN5) for CH0
	AD1CHS0bits.CH0NA = 0;			// MUXA -ve input selection (Vref-) for CH0

    //AD1PCFGH/AD1PCFGL: Port Configuration Register
	AD1PCFGL = 0xFFFF;
    AD1PCFGLbits.PCFG5 = 0;			// AN5 as Analog Input

    
    IFS0bits.AD1IF = 0;				// Clear the A/D interrupt flag bit
    IEC0bits.AD1IE = 0;				// Do Not Enable A/D interrupt 
    AD1CON1bits.ADON = 1;			// Turn on the A/D converter
}

void readz()
{
	AD1CON1bits.DONE=0;
	AD1CON1bits.SAMP=1;

	unsigned int i=0;
	float zf=0;
	
	//Waiting for 10 ms.:P
	for(i=0;i<400;++i);
	AD1CON1bits.SAMP=0;
	while(AD1CON1bits.DONE==0);
	zL=(ADC1BUF0&0x00FF); 			// This is the value of the distance in centimeters.
	zH=(ADC1BUF0>>8)&0x00FF;
	z=(zH<<8)|(zL);
	if(z>=1023)
	{
		z=1023;
		zH=3;
		zL=0xFF;
	}	
	U1TXChar(zH);
	U1TXChar(zL);
	U1TXChar(' ');
	// Code for filter
	//FIFO buffer
	zfifo[0]=z;
    if(t==1)
    {
        zf=fB[0]*zfifo[0];
        t=t+1;
    }     
    else if(t==2)
    {
        zf=fB[0]*zfifo[0]+fB[1]*zfifo[1]-fA[0]*zfilt[0];
        t=t+1;
    }     
    else 
    {
		zf=fB[0]*zfifo[0]+fB[1]*zfifo[1]+fB[2]*zfifo[2]-fA[0]*zfilt[0]-fA[1]*zfilt[1];
	}
	//FIFO buffer updation
    zfifo[2]=zfifo[1];
    zfifo[1]=zfifo[0];
    zfilt[1]=zfilt[0];
    zfilt[0]=zf;
    
    z=zf;
    // Code for polyfit	
	//	B3=[0.4097,-0.1129;0.4968,-0.1290;0.2613,-0.0484;-0.2968,0.1290]';

	// Code for decomposition into characters again
	zL=(z&0x00FF); 		
	zH=(z>>8)&0x00FF;
	U1TXChar(zH);
	U1TXChar(zL);
	U1TXChar(NL);
}	
