#include <p33FJ128MC802.h>
#include "muart.h"
#include "IMU.h"
#include "delay.h"
#include "stdlib.h"
#include "main.h"
#include "pwm.h"
#include "adc.h"

_FOSC(OSCIOFNC_ON & FCKSM_CSDCMD & POSCMD_NONE);	//Oscillator Configuration (clock switching: disabled;
							// failsafe clock monitor: disabled; OSC2 pin function: digital IO;
							// primary oscillator mode: disabled)
_FOSCSEL(FNOSC_FRCPLL);					//Oscillator Selection PLL
//_FOSCSEL(FNOSC_FRC);					//Oscillator Selection no PLL
_FWDT(FWDTEN_OFF);					//Turn off WatchDog Timer
_FGS(GCP_OFF);						//Turn off code protect
_FPOR(FPWRT_PWR1);					//Turn off power up timer

float corrpit=4.8253;

void delay(void)
{
	long int i=0;
	while(i<10000000)	
		++i;
}

#ifdef MAIN_CYCLES
unsigned char flag1c=0,flag2c=0;	// Count the time left with the main
#endif

int main(void)
 {
	unsigned long int pack_wait=0;
    ADPCFG = 0xFFFF;				//make ADC pins all digital	
	
	//PLL setup:
	PLLFBD=38; // M=40
	CLKDIVbits.PLLPOST=0;		// N1=2
	CLKDIVbits.PLLPRE=0;		// N2=2
	OSCTUN=0;					// Tune FRC oscillator, if FRC is used


	while(OSCCONbits.LOCK != 1);// Wait for PLL to lock
	RCONbits.SWDTEN = 0;      	// Disable Watch Dog Timer

	InitUART();
	InitIMU();
	InitPWM();
	InitADC();
	
//	ifreset();
	U1TXStr("Resetted.........................");
	while(1)
	{
		readz();
		pack_wait=0;
//		SENupd();
//		// Put you once per loop instructions here
//		pitch_stabilize();
//		update_motor();	

		//frontmotor(576);
		//backmotor(576);
		//filtertest();
		// Put your looping instructions here
//		while((PKT_OBT==0)&&(pack_wait<PACK_TIMEOUT))
//		{
//// PERMANENT CODE.....		
//			pack_wait++;
//#ifdef MAIN_CYCLES				// Count the time left with the main
//			flag1c++;
//			if(flag1c==255)
//			{
//				flag2c++;	
//			}
//#endif
//		}
//		SENupd2();
//		if(SEN_ERR)
//		{
//			U1TXStr("SEN_ERR ");
//			U1TXChar(SEN_ERR+48);
//			U1TXChar(NL);
//		}
//		U1TXFloat(pit);U1TXChar(' ');U1TXFloat(GYYf);U1TXChar(';');U1TXChar(NL);//U1TXFloat(Wf);U1TXChar(' ');U1TXFloat(Wb);U1TXChar(';');U1TXChar(NL);
#ifdef MAIN_CYCLES				// Count the time left with the main
		U1TXDec(flag2c);U1TXChar(' ');U1TXDec(flag1c);U1TXChar(NL);
		flag1c=0;
		flag2c=0;
#endif
	}
}
