#include "p33FJ128MC802.h"
#include "muart.h"
#include "IMU.h"
#include "stdlib.h"
#include "main.h"
#include "math.h"

//Degree
float yaw=0,pit=0,rol=0;
//Radians
float YAW=0,PIT=0,ROL=0;
float GYX=0,GYY=0,GYZ=0;
float pit_prev=0,pit_sat=11.4592;	// Saturates the pitch if the jump is more than pit_sat
									// Comparison happens in degrees
char flag_angle=0;

char PKT_OBT=0;						// Pkt read from the sensor?
char SEN_ERR=0;						// Let the first error come
char SEN_FREE=1;					// SEN_lock...starting off with free. :P
char inp_str[COUNT_OUT];			// 3*6+3+1+x=20

int indx=0;							// Default location of indx is at the start of the string
char PKT=0;							// I don't want the indx to increment on reset

/*****Filter**********/
float f1_i_b[]={0,0,0};
float f1_o_b[]={0,0,0};
float A[]={-124.4584,46.0231};		// A[0] corresponds to the coeff of y[i-1] as it is
float B[]={5.4588,10.6471,5.4588};	// B[0] corresponds to the coeff of x[i]
float F_DIV=100;					// Divisor
char flag=0;						// Makes sure initialization is proper
float GYYf=0;				// Filtered gyro reading

#define TO_RAD(x) (x * 0.01745329252)  // *pi/180

void InitIMU(void)
/**************************
IMU needs to be told send frame on request
**************************/
{
	U2TXChar('#');U2TXChar('o');U2TXChar('0');
}

float getangle(char *t,float div)
/*********************
Returns	: Angle in degrees
Format	: First the MSByte then LSByte
********************/
{
	float a=0;
	signed int b=0;
	b=(t[0]<<8)+t[1];					// Reconstruct the encoded integer
	a=b/div;							// Reconstruct the angle
#ifdef GET_ANGLE_VERBOSE
	U1TXDec(t[0]);U1TXChar(' ');U1TXDec(t[1]);U1TXChar(' ');U1TXFloat(a);U1TXChar(NL);		
#endif
	return a;
}

void SENupd(void)
{
	if(SEN_FREE)						// If sensor is free
	{
#ifdef SENupd2_Verbose
		U1TXStr("SENupd:Requested");U1TXChar(NL);
#endif	
		SEN_FREE=0;						// Sensor no longer free
		SEN_ERR=0;						// Clear SEN_ERR bit
		flushRX2();						// Just safety sake
		U2TXChar('#');U2TXChar('f');	// Get next frame
		IEC1bits.U2RXIE = 1;			// Enable Recieve Interrupts
	}
#ifdef SENupd2_Verbose
	else
	{
		// Commented because takes too much time to print.
		// U1TXStr("SENupd:Sorry I am busy");U1TXChar(NL);
	}
#endif	
}

void __attribute__((__interrupt__, no_auto_psv)) _U2RXInterrupt(void) // U2RX_ISR
{ 
	char temp=0;
    if(U2STAbits.FERR==1)				// Frame error
    { 
		temp=U2RXREG;	
		indx=0;						// Throw away the frame obtained so far.
#ifdef U2RXInterrupt_Verbose
		U1TXStr("IF");
		U1TXChar(NL);
#endif
    } 
    else if (U2STAbits.OERR==1) 		// Overflow error. 
    { 
        flushRX2();    					// Flushing RX2 and clearing o/f bit inside the 
        								// func.
		indx=0;						// Throw away the frame obtained so far.
#ifdef U2RXInterrupt_Verbose
		U1TXStr("IO");
		U1TXChar(NL);    
#endif	
    } 
	else
	{
		inp_str[indx]=U2RXREG;			// Read into the string.
										// If not reading the pkt, indx is @ 19.
#ifdef U2RXInterrupt_Verbose
		//U1TXStr("IG");U1TXChar(temp);U1TXChar(NL);
#endif
/****************************************************************
inp_str is 					X X X X X X X X X X X X X X X X X X X
           					0 1 2 3 4 5 6 7 8 9 101 2 3 4 5 6 7 8
  indx's default location <-y y , p p , r r , g g , g g , g g ? -> Final.
****************************************************************/
		if(PKT)							// On reset, set to 0. 
										// Set by ISR when '#' is seen and cleared when '?' is 
										// seen
										// Ensures the random writes are overwritten @ 18.
		{
			indx++;
			if(indx==18)				// End of line for the frame.
										// indx left untouched for the subsequent random
										// writes.
			{
				PKT=0;					// Stop incrementing indx
				PKT_OBT=1;				// Got the pkt or whatever it is.
				inp_str[indx]=0;		// In case you want to print it.
				IEC1bits.U2RXIE = 0;	// Interrupt off as we got the pkt.
										// Pkt checks to be done by SENupd2();
			}	
		}
		else if(inp_str[indx]=='#')		// Got the incoming frame's header
		{
			indx=0;						// Bring the pointer to the beginning
			PKT=1;						// Start incrementing indx for the frame reception
#ifdef U2RXInterrupt_Verbose
			U1TXStr("IP");
#endif
		}
	}
	IFS1bits.U2RXIF = 0;				// Serviced the interrupt.
} 

void SENupd2(void)
/*********************
Now to bring in compatibility, I have a general SEN_ERR variable.
Returns 0: If proper communication happened.
Returns 1: Gyro saturated
Returns 2: If markers were not received.
Returns 3: If timed-out
Please remember to flushRX if failed.
*********************/
{
	unsigned long int pack_wait=0;
	while(PKT_OBT==0)					// While the packet is to be obtained
	{
		pack_wait++;
		if(pack_wait>PACK_TIMEOUT)		// Second half of the packet timeout
		{
			SEN_ERR=3;
			U1TXChar('S');
			goto end;
		}
	}
	PKT_OBT=0;							// Packet obtained read
	char t1[2];
	char t2[2];
	char t3[2];
	char t4[2];
	char t5[2];
	char t6[2];
	char temp=1;									
	int i=0;
		
//	Check the string?
	if(inp_str[17]!='?')
	{
		SEN_ERR=2;
		U1TXStr("SENupd:Wrong marker @ 17");
		U1TXChar(NL);
		goto end;
	}
	for(i=2;i<17;i=i+3)
	{
		if(inp_str[i]!=',')
		{
			SEN_ERR=2;
			U1TXStr("SENupd:Wrong marker @ ");U1TXChar(i/10+48);U1TXChar(i-10*(i/10)+48);
	  		U1TXChar(NL);
			goto end;
		}
	}
	
// Print the string?
//	for(i=0;i<COUNT_OUT;++i)
//	{
//		U1TXDec(inp_str[i]);U1TXChar(' ');
//	}	
//	U1TXChar(NL);
	
	i=0;
	do
	{
		t1[i]=inp_str[i];					// Copy bit-by-bit into t1
		++i;
	}while(inp_str[i]!=',');				// before , is copied
	++i;									// Go to the next integer
	temp=i;									// i has the position of the first character of the next number
	do
	{
		t2[i-temp]=inp_str[i];				// Copy bit-by-bit into t1
		++i;
	}while(inp_str[i]!=',');				// before , is copied
	++i;									// Go to the next integer
	temp=i;									// i has the position of the first character of the next number
	do
	{
		t3[i-temp]=inp_str[i];				// Copy bit-by-bit into t1
		++i;
	}while(inp_str[i]!=',');				// before ? is copied	
	++i;									// Go to the next integer
	temp=i;									// i has the position of the first character of the next number
	do
	{
		t4[i-temp]=inp_str[i];				// Copy bit-by-bit into t1
		++i;
	}while(inp_str[i]!=',');				// before ? is copied	
	++i;									// Go to the next integer
	temp=i;									// i has the position of the first character of the next number
	do
	{
		t5[i-temp]=inp_str[i];				// Copy bit-by-bit into t1
		++i;
	}while(inp_str[i]!=',');				// before ? is copied	
	++i;									// Go to the next integer
	temp=i;									// i has the position of the first character of the next number
	do
	{
		t6[i-temp]=inp_str[i];				// Copy bit-by-bit into t1
		++i;
	}while(inp_str[i]!='?');				// before ? is copied
	// Get the angles and gyro decoded
	// YAW,PIT and ROL are extern variables and available everywhere
	// GYX,GYY and GYZ are extern variables and available everywhere
	yaw=getangle(t1,YAWe_DIV);
	pit=getangle(t2,PITe_DIV);
	if(flag_angle)
	{
		if(fabs(pit-pit_prev)>pit_sat)
		{
			pit=pit_prev;
		}
	}
	else
	{
		flag_angle=1;
	}
	pit_prev=pit;
	rol=getangle(t3,ROLe_DIV);
	GYX=getangle(t4,GYRO_DIV);				// Normalized to 40 not 100
	GYY=getangle(t5,GYRO_DIV);
	GYZ=getangle(t6,GYRO_DIV);
	if((GYX==GYRO_SAT)||(GYY==GYRO_SAT)||(GYZ==GYRO_SAT))	// Gyro saturated
	{
#ifdef SENupd_Verbose
		U1TXStr("SENupd: Gyro readings saturated.");
		U1TXFloat(GYX);U1TXFloat(GYY);U1TXFloat(GYZ);U1TXChar(NL);
#endif
		SEN_ERR=1;
		goto end;
	}
	else
	{
#ifdef SENupd_Verbose
		U1TXStr("y=");U1TXFloat(yaw);U1TXStr(" p=");U1TXFloat(pit);U1TXStr(" r=");U1TXFloat(rol);U1TXStr(" gx=");U1TXFloat(GYX);U1TXStr(" gy=");U1TXFloat(GYY);U1TXStr(" gz=");U1TXFloat(GYZ);U1TXChar(NL);
#endif
		// Converting the angles to radians
	    YAW=TO_RAD(yaw);
	    PIT=TO_RAD(pit);
	    ROL=TO_RAD(rol);
	}
	SEN_ERR=0;								// No error
end:
	SEN_FREE=1;								// Free the sensor
}

void filtertest(void)
{
	f1_i_b[0]=f1_i_b[1];
	f1_i_b[1]=f1_i_b[2];
	f1_i_b[2]=GYY;
	f1_o_b[0]=f1_o_b[1];
	f1_o_b[1]=f1_o_b[2];
	f1_o_b[2]=(B[0]*f1_i_b[2]+B[1]*f1_i_b[1]+B[2]*f1_i_b[0]-A[0]*f1_o_b[1]-A[1]*f1_o_b[0])/F_DIV;
	GYYf=f1_o_b[2];
	//U1TXFloat(PIT);U1TXChar(' ');U1TXFloat(GYY);U1TXChar(' ');
	//U1TXFloat(f1_o_b[2]);U1TXChar(';');U1TXChar(NL);
}	
