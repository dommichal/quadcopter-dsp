/**
 * @file rc.c
 * @author Dominik Michalczyk
 * @brief Radio control liblary for decoding
 *        messages
 * @version 0.1
 * @date 2024-01-26
 *
 *
 */
#include <rc.h>

/**
 * @brief decode message
 *        -  msg[0] - Thrust
 *        -  msg[1] - Pitch
 *        -  msg[2] - Yaw
 *        -  msg[3] - Roll
 *        -  msg[4] - Power on
 *        -  msg[5] - Power off
 *        - (msg[6] - msg[8]) - free
 */
void RC_Receive_Message(const uint8_t message[8], RC_t *rc) {
    int8_t *message_int = (int8_t *)message;
    
    if (message_int[0] > ACTIVATION_THRESHOLD_THRUST) {
        rc->controls_inputs[thrust] = message_int[0];
    } else {
        rc->controls_inputs[thrust] = 10;
    }

    if (message_int[1] > ACTIVATION_THRESHOLD ||
        message_int[1] < -ACTIVATION_THRESHOLD) {
        rc->controls_inputs[pitch] = message_int[1] * PITCH_ANGLE_SCALE;
    } else {
        rc->controls_inputs[pitch] = 0;
    }

    if (message_int[2] > ACTIVATION_THRESHOLD ||
        message_int[2] < -ACTIVATION_THRESHOLD) {
        rc->controls_inputs[yaw] = message_int[2] * YAW_ANGLE_SCALE;
    } else {
        rc->controls_inputs[yaw] = 0;
    }

    if (message_int[3] > YAW_ACTIVATION_THRESHOLD ||
        message_int[3] < -YAW_ACTIVATION_THRESHOLD) {
        rc->controls_inputs[roll] = message_int[3] * ROLL_ANGLE_SCALE;
    } else {
        rc->controls_inputs[roll] = 0;
    }

    if (message_int[4] == 1 && rc->power_on == false) {
        rc->power_on =true;
    }

    if (message_int[5] == 1 && rc->power_on == true) {
        rc->power_on = false;
    }
}

uint16_t time_out_cnt = 0;

void RC_Connection_Tick() { time_out_cnt = 0; }

bool RC_Check_Connection() {
    HAL_Delay(1);
    if (time_out_cnt < MAX_CONTROLLER_TIMEOUT) {
        time_out_cnt++;
    }
    return (time_out_cnt >= MAX_CONTROLLER_TIMEOUT);
}

void Lower_Altitude(RC_t *rc) {
    rc->controls_inputs[roll] = 0;
    rc->controls_inputs[pitch] = 0;
    rc->controls_inputs[yaw] = 0;
    while (rc->controls_inputs[thrust] > 10) {
        HAL_Delay(800);
        rc->controls_inputs[thrust]--;
    }
}