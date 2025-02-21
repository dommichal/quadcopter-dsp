/**
 * @file stabilizer.c
 * @author Dominik Michalczyk
 * @brief Stabilizer liblary for stabilizing quadcopter
 * @version 0.1
 * @date 2024-01-26
 */
#include "dsp/stabilizer.h"

#include <assert.h>

#include "dsp/filters.h"
#include "dsp/angle_estimation.h"
#include "dsp/pid.h"
#include "drivers/motors.h"

static PIDController roll_pid, pitch_pid, yaw_pid;

void Stabilizer_init() {
    // Initialize all pids sample times and max_values
    roll_pid.maxOut =   75;
    roll_pid.minOut =  -75;
    roll_pid.maxInt =    5;
    roll_pid.minInt =   -5; 
    
    pitch_pid.maxOut =  75;
    pitch_pid.minOut = -75;
    pitch_pid.maxInt =   5;
    pitch_pid.minInt =  -5; 

    yaw_pid.maxOut =  75;
    yaw_pid.minOut = -75;
    yaw_pid.maxInt =   5;
    yaw_pid.minInt =  -5; 

    roll_pid.sampleTime  =  0.001f;
    pitch_pid.sampleTime =  0.001f;
    yaw_pid.sampleTime   =  0.001f;
    /* Initialize all constants for each pid */

    //roll pid
    roll_pid.tau  =  0.005f;  // 25Hz cutoff freq
    roll_pid.kp   =  2.0f;
    roll_pid.ki   =  0.0f;  
    roll_pid.kd   = -0.35f;
    
    //pitch pid
    pitch_pid.tau =  0.005f; // 25Hz cutoff freq
    pitch_pid.kp  =  2.0f;
    pitch_pid.ki  =  0.0f;
    pitch_pid.kd  = -0.35f;

    //yaw pid
    yaw_pid.tau =  0.008f;  // 20Hz cutoff freq
    yaw_pid.kp  =  2.0f;
    yaw_pid.ki  =  0.0f;
    yaw_pid.kd  =  0.0f;

    PID_init(&roll_pid);
    PID_init(&pitch_pid);
    PID_init(&yaw_pid);
}

void Stabilizer_Update(float angles[2], float angular_velocities[3], int8_t control_inputs[4]){
    assert(NULL != angles);
    assert(NULL != angular_velocities);
    assert(NULL != control_inputs);

    float set_val[3];
    float duty_cycles[3];

    set_val[0] = control_inputs[1]; // roll
    set_val[1] = control_inputs[3]; // pitch
    set_val[2] = control_inputs[2]; // yaw

    /* Angle PID's */
    //roll    
    duty_cycles[0] = PID_Calculate(&pitch_pid, radToDeg(angles[0]), set_val[0]);
    //pitch
    duty_cycles[1] = PID_Calculate(&roll_pid, radToDeg(angles[1]), set_val[1]);
    //yaw
    duty_cycles[2] = PID_Calculate(&yaw_pid, angular_velocities[2], set_val[2]);

    Motors_SetPWM(control_inputs[0], duty_cycles[2], duty_cycles[1], duty_cycles[0]);    
}
