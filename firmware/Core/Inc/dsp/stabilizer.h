#if !defined(STABILIZER)
#define STABILIZER

#include <dsp/filters.h>
#include <dsp/angle_estimation.h>
#include <dsp/pid.h>
#include <drivers/motors.h>

/**
 * @brief Initializes the stabilizer module.
 *
 * This function initializes the stabilizer module and prepares it for operation.
 * It should be called before using any other functions in the stabilizer module.
 */
void Stabilizer_init();

/**
 * @brief Stabilizes the quadcopter using the given angles, angular velocities, and control inputs.
 *
 * This function takes in the current angles, angular velocities, and control inputs of the quadcopter
 * and performs stabilization calculations to maintain stability during flight.
 *
 * @param angles The array of current angles (roll and pitch) of the quadcopter.
 * @param angular_velocities The array of current angular velocities (roll, pitch, and yaw) of the quadcopter.
 * @param control_inputs The array of control inputs (throttle, roll, pitch, and yaw) for the quadcopter.
 */
void Stabilize(float angles[2], float angular_velocities[3], int8_t control_inputs[4]);

#endif // STABILIZER
