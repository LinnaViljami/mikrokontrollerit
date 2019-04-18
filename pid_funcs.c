/*This file has been prepared for Doxygen automatic documentation generation.*/
#ifndef PID_H
#define PID_H

#include "stdint.h"

#define SCALING_FACTOR  128
typedef struct PID_DATA{
int16_t lastProcessValue;
int32_t sumError;
int16_t P_Factor;
int16_t I_Factor;
int16_t D_Factor;
int16_t maxError;
int32_t maxSumError;
} pidData_t;
// Maximum value of variables
#define MAX_INT         INT16_MAX
#define MAX_LONG        INT32_MAX
#define MAX_I_TERM      (MAX_LONG / 2)

// Boolean values
#define FALSE 0
#define TRUE 1

void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, struct PID_DATA *pid);
int16_t pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st);
void pid_Reset_Integrator(pidData_t *pid_st);
#endif


void pid_Init(int16_t p_factor, int16_t i_factor, int16_t d_factor, struct PID_DATA *pid)
// Set up PID controller parameters
{
   // Start values for PID controller
   pid->sumError = 0;
   pid->lastProcessValue = 0;
   // Tuning constants for PID loop
   pid->P_Factor = p_factor;
   pid->I_Factor = i_factor;
   pid->D_Factor = d_factor;
   // Limits to avoid overflow
   pid->maxError = MAX_INT / (pid->P_Factor + 1);
   pid->maxSumError = MAX_I_TERM / (pid->I_Factor + 1);
}


int16_t pid_Controller(int16_t setPoint, int16_t processValue, struct PID_DATA *pid_st)
{
   int16_t error, p_term, d_term;
   int32_t i_term, ret, temp;

   error = setPoint - processValue;

   // Calculate Pterm and limit error overflow
   if (error > pid_st->maxError){
     p_term = MAX_INT;
   }
   else if (error < -pid_st->maxError){
     p_term = -MAX_INT;
   }
   else{
     p_term = pid_st->P_Factor * error;
   }

   // Calculate Iterm and limit integral runaway
   temp = pid_st->sumError + error;
   if(temp > pid_st->maxSumError){
     i_term = MAX_I_TERM;
     pid_st->sumError = pid_st->maxSumError;
	 //pid_Reset_Integrator(pid_st);
   }
   else if(temp < -pid_st->maxSumError){
     i_term = -MAX_I_TERM;
     pid_st->sumError = -pid_st->maxSumError;
	 //pid_Reset_Integrator(pid_st);
   }
   else{
     pid_st->sumError = temp;
     i_term = pid_st->I_Factor * pid_st->sumError;
   }

   // Calculate Dterm
   d_term = pid_st->D_Factor * (pid_st->lastProcessValue - processValue);

   pid_st->lastProcessValue = processValue;

   ret = (p_term + i_term + d_term) / SCALING_FACTOR;
   if(ret > MAX_INT){
     ret = MAX_INT;
   }
   else if(ret < -MAX_INT){
     ret = -MAX_INT;
   }

   return((int16_t)ret);
}

void pid_Reset_Integrator(pidData_t *pid_st)
{
	pid_st->sumError = 0;
}

