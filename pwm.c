#include <p33FJ128MC802.h>
#include "pwm.h"
#include "IMU.h"
#include "muart.h"

#define WMIN 471						// 4500 rpm
#define WMAX 580						// 6500 rpm

// KPIT1 -> Kp*KPIT, KPIT2 -> Kp*KPIT and KPIT is the constant calculated for pitch
//#define HSPD 471						// 4500
//#define KPIT1 117.9523472517103			// 20
//#define KPIT2 11.79523472517103			// 2
//#define KPITi 0.2948808681292757			// 0.05
#define HSPD 523						// 576 5500
#define ZEIG_CORR 0.483;
float KPIT1=26.5393,KPIT2=2.65393,KPITi=0;
//#define KPITi .3618992472495656		// 0.075

//const float front1 = 0.96276915, front2 = 872.1915;	//857 was good
//const float front1 = 0.96276915, front2 = 919.31154;	//857 was good
const float front1 = 0.9628, 	 front2 = 919.3115;//881.31154;	
const float back1  = 0.99330693, back2  = 890.75434;
const float right1 = 0.99335299, right2 = 889.94829;
const float left1  = 0.99484994, left2  = 885.89501;

float Wf=0,Wb=0,Wl=0,Wr=0;
float PITi_err=0;
float gyyb[]={0,0,0,0,0};

void InitPWM(void)
{
	unsigned long int i=0;
	TRISB = TRISB & ~(1<<14);           // make sure PWM pins are set to be outputs
  	TRISB = TRISB & ~(1<<12);           // make sure PWM pins are set to be outputs
  	TRISB = TRISB & ~(1<<10);           // make sure PWM pins are set to be outputs
  	TRISB = TRISB & ~(1<<8);            // make sure PWM pins are set to be outputs
  	
  	asm volatile ("mov #OSCCONL, w1  \n"
				  "mov #0x46, w2     \n"
				  "mov #0x57, w3     \n"
				  "mov.b w2, [w1]    \n"
				  "mov.b w3, [w1]    \n"
				  "bclr OSCCON, #6");
				  
	RPOR7bits.RP14R = 0b10010;		// RP 14 PB 14
    RPOR6bits.RP12R = 0b10011;		// RP 12 PB 12
    RPOR5bits.RP10R = 0b10100;		// RP 10 PB 10
	RPOR4bits.RP8R  = 0b10101;		// RP 08 PB 08....Need to set x0x0x0x0xxxxxxxx
	
	asm volatile ("mov #OSCCONL, w1  \n"
				  "mov #0x46, w2     \n"
				  "mov #0x57, w3     \n"
				  "mov.b w2, [w1]    \n"
				  "mov.b w3, [w1]    \n"
				  "bset OSCCON, #6");
	
  
    P1TCONbits.PTOPS  = 0b0000;  // PWM timer post-scale
    P2TCONbits.PTOPS  = 0b0000;  // PWM timer post-scale
    P1TCONbits.PTCKPS = 0b11;    // PWM timer pre-scale of 1/64
    P2TCONbits.PTCKPS = 0b11;    // PWM timer pre-scale of 1/64
    P1TCONbits.PTMOD  = 0;  	 // PWM operates in free-running Mode continuously
    P2TCONbits.PTMOD  = 0;  	 // PWM operates in free-running Mode continuously
 
    P1TMR  = 0;   				 // PWM counter value, start at 0
    P2TMR  = 0;   				 // PWM counter value, start at 0
    
    /********************************
	/PxTPER	= Fcy/(Fpwm*Scaler)-1
	/	  	= 36850000/(50*64)-1
	/	  	= 11515
	********************************/
    P1TPER = 11515;  			 // PWM Timebase period
    P2TPER = 11515;  			 // PWM Timebase period
  
    PWM1CON1bits.PMOD3 = 1; 	 // PWM in independent mode
    PWM1CON1bits.PMOD2 = 1; 	 // PWM in independent mode
    PWM1CON1bits.PMOD1 = 1; 	 // PWM in independent mode
    PWM2CON1bits.PMOD1 = 1; 	 // PWM in independent mode
    
    PWM1CON1bits.PEN3H = 1; 	 // PWM High pin is enabled
    PWM1CON1bits.PEN2H = 1; 	 // PWM High pin is enabled
    PWM1CON1bits.PEN1H = 1; 	 // PWM High pin is enabled
    PWM2CON1bits.PEN1H = 1; 	 // PWM High pin is enabled
    
    PWM1CON1bits.PEN3L = 0; 	 // PWM Low pin disabled (direction control later?)
    PWM1CON1bits.PEN2L = 0; 	 // PWM Low pin disabled (direction control later?)
    PWM1CON1bits.PEN1L = 0; 	 // PWM Low pin disabled (direction control later?)
    PWM2CON1bits.PEN1L = 0; 	 // PWM Low pin disabled (direction control later?)
   
  	P1DC1 = 1151; 				 // PWM#1 Duty Cycle register Left  while facing the Regulator
  			                     //    Clockwise
  	P1DC2 = 1151; 				 // PWM#2 Duty Cycle register Front while facing the Regulator
  								 //    Clockwise
  	P1DC3 = 1151; 				 // PWM#3 Duty Cycle register Back  while facing the Regulator
  								 //  Counter clockwise
  	P2DC1 = 1151; 				 // PWM#1 Duty Cycle register Right while facing the Regulator
  								 //  Counter clockwise
    
    P1TCONbits.PTEN   = 1;  	 // Enable PWM Timerbase 
    P2TCONbits.PTEN   = 1;  	 // Enable PWM Timerbase 
    
//    U1TXStr("Delay macha delay");
//    U1TXChar(NL);
//    for(i=0;i<25000000;++i);
//	  ifreset();
}

void frontmotor(float w)
{
	unsigned int PWM=0;
	if((w<WMAX)&&(w>WMIN))
	{	
		PWM=(front1*w+front2)+0.5; // 2 times because the current setting is for 11515 base
									 // but our counting mode enables 2x pwm settings
		P1DC2 = PWM;
	}
	else if(w<=WMIN)
	{	
		PWM=(front1*WMIN+front2)+0.5;			 // 0 rpm
		P1DC2 = PWM;
	}
	else							 // w>WMAX
	{
		PWM=(front1*WMAX+front2)+0.5;// 2 times because the current setting is for 11515 base
									 // but our counting mode enables 2x pwm settings
		P1DC2 = PWM;
#ifdef MOTOR_Verbose
		U1TXStr("Front motor maxed out");
#endif
	}	
}

void backmotor(float w)
{
	unsigned int PWM=0;
	if((w<WMAX)&&(w>WMIN))
	{	
		PWM=(back1*w+back2)+0.5;
		P1DC3 = PWM;
	}
	else if(w<=WMIN)
	{	
		PWM=(back1*WMIN+back2)+0.5;			 // 0 rpm
		P1DC3 = PWM;
	}
	else
	{
		PWM=(back1*WMAX+back2)+0.5;// 2 times because the current setting is for 11515 base
									 // but our counting mode enables 2x pwm settings
		P1DC3 = PWM;
#ifdef MOTOR_Verbose
		U1TXStr("Back motor maxed out");
#endif
	}	
}

void leftmotor(float w)
{
	unsigned int PWM=0;
	if((w<WMAX)&&(w>WMIN))
	{	
		PWM=(left1*w+left2)+0.5;
		P1DC1 = PWM;
	}	
	else if(w<=WMIN)
	{	
		PWM=(left2)+0.5;			 // 0 rpm
		P1DC1 = PWM;
	}
	else
	{
		PWM=(left1*WMAX+left2)+0.5;// 2 times because the current setting is for 11515 base
									 // but our counting mode enables 2x pwm settings
		P1DC1 = PWM;
#ifdef MOTOR_Verbose
		U1TXStr("Back motor maxed out");
#endif
	}
}

void rightmotor(float w)
{
	unsigned int PWM=0;
	if((w<WMAX)&&(w>WMIN))
	{
		PWM=(right1*w+right2)+0.5;
		P2DC1 = PWM;
	}	
	else if(w<=WMIN)
	{	
		PWM=(right2)+0.5;			 // 0 rpm
		P2DC1 = PWM;
	}
	else
	{
		PWM=(right1*WMAX+right2)+0.5;// 2 times because the current setting is for 11515 base
									 // but our counting mode enables 2x pwm settings
		P2DC1 = PWM;
#ifdef MOTOR_Verbose
		U1TXStr("Right motor maxed out");
#endif
	}
}

void pitch_stabilize(void)
{
	//PITi_err=PITi_err+PIT;						// Error = PIT....the negative 
													// is there in the expression
	filtertest();
	float f = -KPIT1*PIT-KPIT2*GYYf;				//-KPITi*PITi_err;
	Wf=HSPD+f;       
	Wb=HSPD-f;
}	

void update_motor()
{
	frontmotor(Wf);
	backmotor(Wb);
	leftmotor(Wl);
	rightmotor(Wr);
}

void zeig(void)
{	
	char temp=U1RXChar();
	if(temp=='+')
	{
		KPIT1=KPIT1+ZEIG_CORR;
	}
	else if(temp=='-')
	{
		KPIT1=KPIT1-ZEIG_CORR;
	}
	KPIT2=0;
	KPITi=0;
	pitch_stabilize();
	update_motor();
}
