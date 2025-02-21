/**
 * @author Kacper Filipek & Dominik Michalczyk
 * @brief Module responsible for estimating rotation angles
 *        using accelerometer and gyroscope data.
 * @date  10-09-2023
 */

#include "dsp/angle_estimation.h"

#include <math.h>
#include <assert.h>

enum {
  X = 0,
  Y,
  Z,
};

enum {
  ROLL = 0,
  PITCH,
  YAW,
};

#ifdef KALMAN
static KalmanFilter kalmanPitch;
static KalmanFilter kalmanRoll;
#elif
static float complementartSampling, filterAlpha;
#endif

static IIR_filter_t iir;

static float filter_acc_x_in[2] = {0};
static float filter_acc_x_out[2] = {0};

static float filter_acc_y_in[2] = {0};
static float filter_acc_y_out[2] = {0};

static float filter_acc_z_in[2] = {0};
static float filter_acc_z_out[2] = {0};

void Euler_ComputeAngles(float acceleration[3], float angles[2]) {
  angles[ROLL] = atanf(acceleration[Y] / acceleration[Z]);
  angles[PITCH] = atanf(acceleration[X] / sqrtf(acceleration[Y] * acceleration[Y] + acceleration[Z] * acceleration[Z]));
}

void Euler_ComputeAngularVelocities(float angleChange[3], float angles[2], const float gyro[3]) {
  float sin_psi = sinf(angles[ROLL]);
  float cos_psi = cosf(angles[ROLL]);
  float cos_theta = cosf(angles[PITCH]);
  float tan_theta = tanf(angles[PITCH]);

  angleChange[X] = -gyro[X] + tan_theta * (sin_psi * gyro[Y] + cos_psi * gyro[Z]);
  angleChange[Y] = cos_psi * gyro[Y] - sin_psi * gyro[Z];
  angleChange[Z] = (sin_psi / cos_theta) * gyro[Y] + (cos_psi / cos_theta) * gyro[Z];
}

void Complementary_CalculateRollAndPitch(float angles[2], float acc_angles[2], float angleChange[3], float dt, float alpha) {
  angles[ROLL] += degToRad(angleChange[ROLL]) * dt;
  angles[PITCH] += degToRad(angleChange[PITCH]) * dt;

  angles[ROLL] = alpha * acc_angles[ROLL] + (1 - alpha) * angles[ROLL];
  angles[PITCH] = alpha * acc_angles[PITCH] + (1 - alpha) * angles[PITCH];
}

void Kalman_Init(KalmanFilter *kalman) {
  kalman->extrapolationTerm = kalman->sampling_time * kalman->sampling_time * kalman->angular_velocity_variance;

  /* initial guess */
  kalman->variancePrediction = sqrt(kalman->angleVariance);
}

void Kalman_Estimate(KalmanFilter *kalman, float *kalmanState, float measurement, float velocity) {
  /* predict current state */
  kalman->state = kalman->state + kalman->sampling_time * velocity;
  /* calculate current variance */
  kalman->variancePrediction = kalman->variancePrediction + kalman->extrapolationTerm;
  /* update kalman gain */
  kalman->kalman_gain = kalman->variancePrediction / (kalman->variancePrediction + kalman->angleVariance);
  /* predict kalman angle */
  kalman->state = kalman->state + kalman->kalman_gain * (measurement - kalman->state);
  /* update variance */
  kalman->variancePrediction = (1 - kalman->kalman_gain) * kalman->variancePrediction;
  /* save last value */
  *kalmanState = kalman->state;
}

void Estimator_Init(float dt) {
  // Low pass filter for accelerometer data
  iir.samplingTime = dt;
  iir.tau = 0.04f;
  Low_Pass_IIR_Filter_Init(&iir);

#ifdef KALMAN
  kalmanPitch.sampling_time = dt;
  kalmanPitch.state = 0;
  kalmanPitch.angular_velocity_variance = degToRad(4) * degToRad(4);
  kalmanPitch.angleVariance = degToRad(3) * degToRad(3);
  Kalman_Init(&kalmanPitch);
  
  kalmanRoll.sampling_time = dt;
  kalmanRoll.state = 0;
  kalmanRoll.angular_velocity_variance = degToRad(4) * degToRad(4);
  kalmanRoll.angleVariance = degToRad(3) * degToRad(3);
  Kalman_Init(&kalmanRoll);
#elif
  // Complementary filter
  filterAlpha = 0.001f;
  complementartSampling = dt;
#endif
}

void Estimator_DetermineAngles(float angles[2], float angularRates[3], const float acceleration[3], const float gyro[3]) {
  float acc_angles[2];
  float filtered_acc[3];

  filter_acc_x_in[0] = acceleration[X];
  Low_Pass_IIR_Filter(&iir, filter_acc_x_out, filter_acc_x_in);
  filtered_acc[X] = filter_acc_x_out[0];

  filter_acc_y_in[0] = acceleration[Y];
  Low_Pass_IIR_Filter(&iir, filter_acc_y_out, filter_acc_y_in);
  filtered_acc[Y] = filter_acc_y_out[0];

  filter_acc_z_in[0] = acceleration[Z];
  Low_Pass_IIR_Filter(&iir, filter_acc_z_out, filter_acc_z_in);
  filtered_acc[Z] = filter_acc_z_out[0];

  Euler_ComputeAngles(filtered_acc, acc_angles);

  Euler_ComputeAngularVelocities(angularRates, angles, gyro);

#ifdef KALMAN
  Kalman_Estimate(&kalmanRoll, &angles[ROLL], acc_angles[ROLL], degToRad(angularRates[X]));
  Kalman_Estimate(&kalmanPitch, &angles[PITCH], acc_angles[PITCH], degToRad(angularRates[Y]));
#else
  angles[ROLL] += degToRad(angularRates[ROLL]) * complementartSampling;
  angles[PITCH] += degToRad(angularRates[PITCH]) * complementartSampling;

  angles[ROLL] = filterAlpha * acc_angles[ROLL] + (1 - filterAlpha) * angles[ROLL];
  angles[PITCH] = filterAlpha * acc_angles[PITCH] + (1 - filterAlpha) * angles[PITCH];
#endif
}
