/**
 * @file motors.c
 * @author Dominik Michalczyk
 * @brief 
 * @version 0.1
 * @date 2024-01-26
 */

#include "drivers/motors.h"

#include "gpio.h"
#include "tim.h"

#include <stdbool.h>
#include <stdint.h>

void Motors_Run(int8_t thrust, int8_t yaw, int8_t pitch, int8_t roll, bool power_on){
    Motors_SetPWR(thrust, yaw, pitch, roll);
    Motors_Switch(power_on);
}

void Motors_Switch(bool power_on){
  if(power_on){
    //Start all PWM chanels
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);   
  }else{
    //Stop all PWM chanels
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
  }
}

void Motors_SetPWR(uint8_t thrust, int8_t yaw, int8_t pitch, int8_t roll)
{
  //Motor 1 PWM duty cycle
  TIM2->CCR1 = THRUST_CONST * thrust - PITCH_CONST * pitch + YAW_CONST * yaw - ROLL_CONST * roll;
  //Motor 2 PWM duty cycle
  TIM2->CCR2 = THRUST_CONST * thrust + PITCH_CONST * pitch - YAW_CONST * yaw - ROLL_CONST * roll;
  //Motor 3 PWM duty cycle
  TIM2->CCR3 = THRUST_CONST * thrust + PITCH_CONST * pitch + YAW_CONST * yaw + ROLL_CONST * roll;
  //Motor 4 PWM duty cycle
  TIM2->CCR4 = THRUST_CONST * thrust - PITCH_CONST * pitch - YAW_CONST * yaw + ROLL_CONST * roll;
}