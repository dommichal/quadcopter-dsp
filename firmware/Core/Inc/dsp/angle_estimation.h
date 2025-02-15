#if !defined(ANGLE_ESTIMATION)
#define ANGLE_ESTIMATION
#include "math.h"
#include "stdbool.h"
#include <filters.h>

#define degToRad(angleInDegrees) ((angleInDegrees) * 3.141592f / 180.0f)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0f / 3.141592f)

#define AX_OFFSET 0.05f

#ifndef COMPLEMENTARY
#define KALMAN
#endif

typedef struct {
  float kalman_angle;
  float kalman_gain;
  float variancePrediction;
  float extrapolationTerm;

  float sampling_time;
  float angular_velocity_variance;
  float angleVariance;
} KalmanFilter;

/**
 * @brief Calculate fixed frame Euler roll and pitch
 * @note Remap gyro angular velocity (mpu on pcb orientation)
 *       gyro_x = -gyro_x;
 */
void Euler_ComputeAngles(float acc_buf[3], float angles[2]);

/**
 * @brief calculates the rotational speed of Euler angles
 * @note Remap gyro angular velocity (mpu on pcb orientation)
 *       gyro_x = -gyro_x;
 * @param angle_change Euler angular velocities
 * @param angles current estimation of euler angles [roll, pitch, yaw]
 * @param gyro gyro inputs [x, y, z]
 */
void Euler_ComputeAngularVelocities(float angle_change[3], float angles[2], const float gyro[3]);

/**
 * @brief estimates Euler angles using a complementary filter
 */
void Complementary_CalculateRollAndPitch(float angles[2], float acc_angles[2], float angle_change[3], float dt, float alpha);

/**
 * @brief one dimensional Kalman filter
 */
void Kalman_Init(KalmanFilter *kalman);

void Kalman_Estimate(KalmanFilter *kalman, float *kalman_state, float measurement, float velocity);

void Estimator_Init(float dt, float alpha, float tau);

/**
 * @brief Calculates Euler angles estimates using
 *        1D Kalman or complementary filter
 */
void Estimator_DetermineAngles(float angles[2], float angular_velocities[3], const float acc_buf[3], const float gyro_buf[3]);

#endif // ANGLE_ESTIMATION
