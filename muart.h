#ifndef _muart_h
#define _muart_h

//Verbose commands
#define xflushRX2_Verbose			
#define xU1RXChar_Verbose			// Superficial
#define xU2RXChar_Verbose			// Superficial
#define xU2RXChar_Verbose_2			// Character by character TCR maps here. 
									// Couldn't put the U2RXChar identifier coz 
									// of time constrains.

/*********************************
// Load a value into Baud Rate Generator.  Example is for 9600.
// See section 19.3.1 of datasheet.
//  U1BRG = (Fcy / (16 * BaudRate)) - 1
//  U1BRG = (36850000 / (16 * 9600)) - 1
//  U1BRG = 238.908854 //Round to 239
	
239    9600		
159	   14400	
119    19200
79     28800
59	   38400
39	   57600
19	   115200
**********************************/
#define U1SPEED 19				// Bluetooth
#define U2SPEED 59				// IMU

#define NL 13

extern char U2RX_ERR;

void InitUART(void);
void U1TXChar(char);
void U1TXStr(char *);
char U1RXChar();
void U1TXFloat(float);
void U1TXDec(unsigned char);
void U2TXChar(char);
void U2TXStr(char *);
char U2RXChar();
void flushRX2();

void ifreset(void);

#endif
