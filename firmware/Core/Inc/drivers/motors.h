#if !defined(MOTORS)
#define MOTORS

#include <stdbool.h>
#include <stdint.h>

#define THRUST_CONST 10
#define YAW_CONST 1
#define PITCH_CONST 4.7
#define ROLL_CONST 4.7

#define MAX_CONTROLLER_VALUE 100

/**
 * @brief Runs the motors with the specified thrust, yaw, pitch, roll, and power
 * state.
 *
 * This function controls the motors of the quadcopter by setting the desired
 * thrust, yaw, pitch, and roll values. The power_on parameter determines
 * whether the motors should be turned on or off.
 *
 * @param thrust The desired thrust value.
 * @param yaw The desired yaw value.
 * @param pitch The desired pitch value.
 * @param roll The desired roll value.
 * @param power_on The power state of the motors (0 for off, 1 for on).
 */
void Motors_Run(int8_t thrust, int8_t yaw, int8_t pitch, int8_t roll,
                bool power_on);

/**
 * @brief Sets the power levels for the motors.
 *
 * This function sets the power levels for the motors based on the provided
 * thrust, yaw, pitch, and roll values.
 *
 * @param thrust The thrust level.
 * @param yaw The yaw level.
 * @param pitch The pitch level.
 * @param roll The roll level.
 */
void Motors_SetPWR(uint8_t thrust, int8_t yaw, int8_t pitch, int8_t roll);

/**
 * @brief Switches the motors on or off.
 *
 * This function is used to control the power state of the motors.
 *
 * @param power_on A flag indicating whether to turn the motors on or off.
 *                 Set to 1 to turn the motors on, and 0 to turn them off.
 */
void Motors_Switch(bool power_on);

#endif // MOTORS
