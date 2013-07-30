#ifndef _pwm_h
#define _pwm_h

extern float Wf,Wb,Wl,Wr;
//extern float KPIT1,KPIT2,KPITi;

void InitPWM(void);
void frontmotor(float);
void backmotor(float);
void leftmotor(float);
void rightmotor(float);

void zeig(void);
void pitch_stabilize();
void update_motor();

#endif
