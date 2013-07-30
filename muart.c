#include <p33FJ128MC802.h>
#include <stdio.h>
#include "muart.h"

#define TIMEOUT 1000000						// @ 40 MHz, this translates to 2.5 ms.

char U2RX_ERR=0;

void InitUART(void) 
{
	TRISB = TRISB & ~(1<<13);			 	// 13th pin is output
	TRISB = TRISB |  (1<<11);				// 11th pin is input
	TRISB = TRISB & ~(1<<9);			 	// 9th pin is output
	TRISB = TRISB |  (1<<7);				// 7th pin is input
	asm volatile ("mov #OSCCONL, w1  \n"
				  "mov #0x46, w2     \n"
				  "mov #0x57, w3     \n"
				  "mov.b w2, [w1]    \n"
				  "mov.b w3, [w1]    \n"
				  "bclr OSCCON, #6");
				  

	RPINR18bits.U1RXR = 7;					//UART RX of chip mapped to RPin 7
	RPOR4bits.RP9R =0b00011 ;             	//UART TX mapped to RPin 9
    RPOR6bits.RP13R = 0b00101;            	//UART2 TX mapped to RPin 13 (B.13)
    RPINR19bits.U2RXR = 11;       	 		//UART2 RX mapped to RPin 11 (B.11)
	
	asm volatile ("mov #OSCCONL, w1  \n"
				  "mov #0x46, w2     \n"
				  "mov #0x57, w3     \n"
				  "mov.b w2, [w1]    \n"
				  "mov.b w3, [w1]    \n"
				  "bset OSCCON, #6");
/***************************************
Configuration:
U1MODE
Continue operation in IDLE Mode | No IR Translation | UxRTS in Simplex MODE | Only Tx,Rx under our control | 
No wake-up, since no sleep | Disable Loopback | No Baud rate detection | IDLE STATE is 1 in RX | Low-speed mode |
8-bit communication with 1 stop bit and no parity

U1STA
Interrupt when atleast 1 bit is empty in TX Buffer | Sync break transmission disabled | Interrupt when character received |
***************************************/
	// configure U1MODE
	U1MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	//U2MODEbits.notimplemented;	// Bit14
	U1MODEbits.USIDL = 0;	// Bit13 Continue in Idle
	U1MODEbits.IREN = 0;	// Bit12 No IR translation
	U1MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
	//U2MODEbits.notimplemented;	// Bit10
	U1MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U1MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
	U1MODEbits.LPBACK = 0;	// Bit6 No Loop Back
	U1MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
	U1MODEbits.URXINV = 0;	// Bit4 IdleState = 1  (for dsPIC)
	U1MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
	U1MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U1MODEbits.STSEL = 0;	// Bit0 One Stop Bit
	
	U1BRG = U1SPEED;

	// Load all values in for U1STA SFR
	U1STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U1STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
	U1STAbits.UTXISEL0 = 0;	//Bit13 Other half of Bit15
	//U2STAbits.notimplemented = 0;	//Bit12
	U1STAbits.UTXBRK = 0;	//Bit11 Disabled
	U1STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
	U1STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
	U1STAbits.TRMT = 0;		//Bit8 *Read Only bit*
	U1STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
	U1STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
	U1STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
	U1STAbits.PERR = 0;		//Bit3 *Read Only Bit*
	U1STAbits.FERR = 0;		//Bit2 *Read Only Bit*
	U1STAbits.OERR = 0;		//Bit1 *Read Only Bit*
	U1STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

	U1MODEbits.UARTEN = 1;	// And turn the peripheral on
	U1STAbits.UTXEN = 1;	//Whenever, U1TXReg is written, send it off.
/***************************************
Configuration:
U2MODE
Continue operation in IDLE Mode | No IR Translation | UxRTS in Simplex MODE | Only Tx,Rx under our control | 
No wake-up, since no sleep | Disable Loopback | No Baud rate detection | IDLE STATE is 1 in RX | Low-speed mode |
8-bit communication with 1 stop bit and no parity

U2STA
Interrupt when atleast 1 bit is empty in TX Buffer | Sync break transmission disabled | Interrupt when character received |
***************************************/
	// configure U1MODE
	U2MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	//U2MODEbits.notimplemented;	// Bit14
	U2MODEbits.USIDL = 0;	// Bit13 Continue in Idle
	U2MODEbits.IREN = 0;	// Bit12 No IR translation
	U2MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
	//U2MODEbits.notimplemented;	// Bit10
	U2MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U2MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
	U2MODEbits.LPBACK = 0;	// Bit6 No Loop Back
	U2MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
	U2MODEbits.URXINV = 0;	// Bit4 IdleState = 1  (for dsPIC)
	U2MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
	U2MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U2MODEbits.STSEL = 0;	// Bit0 One Stop Bit
	
	U2BRG = U2SPEED;

	// Load all values in for U1STA SFR
	U2STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U2STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
	//CHANGE HERE........v
	U2STAbits.UTXISEL0 = 0;	//Bit13 Other half of Bit15
	//U2STAbits.notimplemented = 0;	//Bit12
	U2STAbits.UTXBRK = 0;	//Bit11 Disabled
	U2STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
	U2STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
	U2STAbits.TRMT = 0;		//Bit8 *Read Only bit*
	U2STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
	U2STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
	U2STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
	U2STAbits.PERR = 0;		//Bit3 *Read Only Bit*
	U2STAbits.FERR = 0;		//Bit2 *Read Only Bit*
	U2STAbits.OERR = 0;		//Bit1 *Read Only Bit*
	U2STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

	IPC7 = IPC7|(4<<12);	// Mid Range Interrupt Priority level, no urgent reason
							// 0x4000

	IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	IEC1bits.U2RXIE = 0;	// Do not enable Recieve Interrupts

	U2MODEbits.UARTEN = 1;	// And turn the peripheral on
	U2STAbits.UTXEN = 1;	//Whenever, U2TXReg is written, send it off.
}

void U1TXChar(char data)
/**************************
Prints a character into the UART buffer -> Bluetooth
**************************/
{
	//Wait for TXREG Buffer to become available
	while(U1STAbits.UTXBF);
	U1TXREG = data;
}

void U1TXStr(char* str)
/**************************
Prints the string into the UART buffer -> Bluetooth
Requires the string to end @ 0.
**************************/
{
  	while (*str)					// Till you hit ASCII 0.
    {
		U1TXChar(*str++);
	}
}

void U1TXFloat(float temp)
/**************************
Prints a float into the UART buffer -> Bluetooth
**************************/
{
	char temp_str[19];
	int i=0;
	sprintf(temp_str,"%.2f",temp);
	while((temp_str[i])!='\0')
	{
		//Wait for TXREG Buffer to become available
		while(U1STAbits.UTXBF);

		//Write data
		U1TXREG=temp_str[i];

		//Next goto char
		i++;
	}
}

void U1TXDec(unsigned char temp)
/**************************
Prints the decimal representation of the char 
in ASCII into the UART buffer -> Bluetooth
**************************/
{
	char temp_str[19];
	int i=0;
	sprintf(temp_str,"%u",temp);					// Unsigned decimal
	while((temp_str[i])!='\0')
	{
		//Wait for TXREG Buffer to become available
		while(U1STAbits.UTXBF);

		//Write data
		U1TXREG=temp_str[i];

		//Next goto char
		i++;
	}
}
		
char U1RXChar(void)
/**************************
Receives a char from the UART buffer -> Bluetooth
**************************/
{
	long int i=0;
	char flag=0;
	while(U1STAbits.URXDA==0)
	{
		++i;
		if(i>TIMEOUT)
			return 0;
	};
	// So no timeout, eh? Good let us see what we got.
	
	/* check for receive errors */
	if(U1STAbits.FERR == 1)
	{
#ifdef U1RXChar_Verbose
		U1TXStr("U1RXChar:Frame Error. Returning a null character.");
		U1TXChar(NL);
#endif
		flag=U1RXREG;		// You need to read the buffer to clear the flag
		return 0;
	}
	else if(U1STAbits.OERR == 1)
	{
#ifdef U1RXChar_Verbose
		U1TXStr("U1RXChar:Overflow Error. Throwing them away.");
		U1TXChar(NL);
#endif
		/* Clear the overrun error to keep uart receiving by reading out. Flushing */
		flag=U1RXREG;flag=U1RXREG;flag=U1RXREG;flag=U1RXREG;
		U1STAbits.OERR = 0;
		return 0;
	}
	return(U1RXREG);
}

		
void U2TXChar(char data)
/**************************
Prints the character into the UART buffer -> IMU
**************************/
{
	//Wait for TXREG Buffer to become available
	while(U2STAbits.UTXBF);
	U2TXREG = data;
}

void U2TXStr(char* str)
/**************************
Prints the string into the UART buffer -> IMU
Requires a ASCII 0 at the end
**************************/
{
  	while (*str)
    {
		U2TXChar(*str++);
	}
}

char U2RXChar(void)
/**************************
Receives a character from the UART buffer -> IMU
U2RX_ERR is set incase error occurs to permit the passing of 0
**************************/
{
	U2RX_ERR=0;								// Set the error flag 0 by default
	long int i=0;
	char temp=0;
	while(U2STAbits.URXDA==0)
	{
		++i;
		if(i>TIMEOUT)
		{	
#ifdef U2RXChar_Verbose
			U1TXStr("U2RXChar:Timed out");
			U1TXChar(NL);
			U1TXStr("U2RXChar:Returning a null character.");
			U1TXChar(NL);
#endif
			U2RX_ERR=1;
			return 0;
		}
	};
	// So no timeout eh? Let us see what we got...
	/* check for receive errors */
	if(U2STAbits.FERR == 1)
	{
#ifdef U2RXChar_Verbose
		U1TXStr("U2RXChar:Frame Error. Returning a null character.");
		U1TXChar(NL);
#endif
		temp=U2RXREG;							// Reading the register to clear the bit.
		U2RX_ERR=1;
		return 0;
	}
	else if(U2STAbits.OERR == 1)
	{
#ifdef U2RXChar_Verbose
		U1TXStr("U2RXChar:Overflow Error. Throwing them away without reading.");
		U1TXChar(NL);
#endif
		flushRX2();								// Flushing the buffer
												// flushRX2 will take care of the mandatory
												// clearing of the U2STAbits.OERR bit
		U2RX_ERR=1;
		return 0;

	}
	temp=U2RXREG;								// Data is fine.
	U2RX_ERR=0;									// No error. :)
#ifdef U2RXChar_Verbose_2
	U1TXChar(temp);U1TXChar('|');
	//U1TXDec(temp);							// Causes overflow error to occur @ IMU
												// baud rate. I have only leeway for 3
												// characters as the speed of UART1 is only
												// 3 times more
	U1TXChar(NL);
#endif
	return temp;
}

void flushRX2(void)
/**************************
Flush the UART buffer -> IMU
**************************/
{
	char temp=0;
#ifdef flushRX2_Verbose
	U1TXStr("flushRX2: You called me?");
	while(U2STAbits.RIDLE==0)
	{
		U1TXStr("flushRX2:Waiting for idle Receiver");// Wait for the reciever to be idle
		U1TXChar(NL);
	}	
	if(U2STAbits.URXDA==1)							  // If buffer has data
	{
		U1TXStr("flushRX2:Flushing => ");
		U1TXChar(NL);
	}
#endif
	while(U2STAbits.URXDA==1)						  // While buffer has data
	{
		temp=U2RXREG;
#ifdef flushRX2_Verbose
		U1TXStr("flushRX2:");U1TXChar(temp);U1TXChar(' ');U1TXDec(temp);	
		U1TXChar(NL);
#endif
	}
	// Emptied the buffer.
	if(U2STAbits.OERR == 1)
	{
		U2STAbits.OERR=0;							  // Clear the U2STAbits.OERR 
													  // (overflow) bit
#ifdef flushRX2_Verbose
		U1TXStr("flushRX2:Cleared the buffer.");	 
		U1TXChar(NL);
#endif
	}
}

void matlab_reset(void)
{
	while(U1RXChar()!='a')
	{
		U1TXStr("A");
	}
}

void ifreset(void)
{
	while(U1RXChar()!='a')
	{
		U1TXStr("Press 'a' to continue");
		U1TXChar(NL);
	}
	if(!(RCON & 0x0003))				// BOR and POR alone are set on a power reset
	{
		U1TXStr("RCON -> ");
		U1TXDec((RCON>>8) & 0x00FF);
		U1TXChar(' ');
		U1TXDec(RCON & 0x00FF);
		U1TXChar(NL);
		while(U1RXChar()!='c')
		{
			U1TXStr("Press 'c' to continue");
			U1TXChar(NL);
		}
	
	}
	RCON=0x0000;
}	
/*********Testing TXString***********
	char str[]="Abraham rocks!!!";
	while(1)
	{
		U2TXStr(str);
		U2TXChar(NL);
	}
************************************/
/*********Testing TXChar*************
	while(1) {
		for(i=68;i<78;++i)
		{
			TXChar(i);
		}
	}
************************************/
/*********Testing RXChar************
	while(1)
	{
		U2TXChar('N');
		a=U2RXChar();
		U2TXChar(a+1);
		U2TXChar(NL);
	}
************************************/
