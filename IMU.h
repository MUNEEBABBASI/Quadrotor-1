#ifndef _IMU_h					//Make sures no double definition
#define _IMU_h

#define COUNT_OUT 19			// Max no. of characters in the IMU output. (2+1)*6+1
#define GYRO_SAT 10				// Gyro saturation @  10. Hence, divider is 3200.
								// Gyro saturation @ 100. Hence, divider is 320.
#define GYRO_DIV 3200
#define YAWe_DIV 160
#define PITe_DIV 320
#define ROLe_DIV 160

//Verbose commands
#define xGET_ANGLE_VERBOSE
#define xSENupd_Verbose			// Leave this on to see IMU output
#define xU2RXInterrupt_Verbose	
#define xSENupd2_Verbose		// Controls the interrupt's SENupd's verbose too.

// Quantity in degrees
extern float YAW,ROL,PIT;
// Quantity in radian
extern float yaw,pit,rol;
extern float GYX,GYY,GYZ;
extern float GYYf;				// Filtered Gyro Y 

extern char SEN_ERR;			// SEN_ERR status
extern char SEN_FREE;			// SEN_lock
extern char PKT_OBT;

void InitIMU(void);
void SENupd(void);
void SENupd2(void);				
float getangle(char *,float);
void filtertest(void);

#endif
