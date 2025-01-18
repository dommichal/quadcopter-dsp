/**
 * @author Kacper Filipek & Dominik Michalczyk
 * @date  10-09-2023
 */

#include <dsp/angle_estimation.h>

/**
 * @brief Calculate fixed frame Euler roll and pitch 
 * @details [ax, ay, az] = (rotation matrix form fixed frame to body frame) * (g vector) 
 *          ax = g * sin(pitch), ay = -g * sin(roll) * cos(pitch), az = -g * cos(roll) * cos(pitch) 
  */
void Calculate_Angles_Acc_Euler(float acc_buf[3], float angles[2]){
  const int x    = 0, y     = 1, z   = 2;
  //remove offset
  acc_buf[x] = acc_buf[x] - AX_OFFSET;

  //cannot divide by 0
  if(acc_buf[z] == 0){
    acc_buf[z] = 0.000001f;
  }
  
  angles[0] = atanf(acc_buf[y]/acc_buf[z]);

  //sin must be in <-1, 1>
  if(acc_buf[x] > 1){
    acc_buf[x] = 1;
  }
  if(acc_buf[x] < -1){
    acc_buf[x] = -1;
  }

  angles[1] = asinf(acc_buf[x]);
}

/**
 * @brief Calculate fixed frame Euler roll and pitch 
 * @note Remap gyro angular velocity (mpu on pcb orientation)
 *       gyro_x = -gyro_x;
  */
void Calculate_Angles_acc(float acc_buf[3], float angles[2]){
  const int x    = 0, y     = 1, z   = 2;
  
  angles[0] = atanf(acc_buf[y]/acc_buf[z]);
  angles[1] = atanf(acc_buf[x]/sqrtf(acc_buf[y] * acc_buf[y] + acc_buf[z] * acc_buf[z]));
}

/**
 * @brief calculates the rotational speed of Euler angles
 * @note Remap gyro angular velocity (mpu on pcb orientation)
 *       gyro_x = -gyro_x;
 * @param angle_change Euler angular velocities
 * @param angles current estimation of euler angles [roll, pitch, yaw]
 * @param gyro gyro inputs [x, y, z]
 */

void Calculate_Angular_Velocities(float angle_change[3], float angles[2], const float gyro[3]){
  const int roll = 0, pitch = 1;
  const int x    = 0, y     = 1, z   = 2;

  float sin_psi   = sinf(angles[roll]);
  float cos_psi   = cosf(angles[roll]);
  float cos_theta = cosf(angles[pitch]);
  float tan_theta = tanf(angles[pitch]);  

  angle_change[0] = -gyro[x] + tan_theta * (sin_psi  * gyro[y] + cos_psi * gyro[z]);
  angle_change[1] = cos_psi * gyro[y] - sin_psi * gyro[z];
  angle_change[2] = (sin_psi / cos_theta) * gyro[y] + (cos_psi / cos_theta) * gyro[z];
}

/**
 * @brief estimates Euler angles using a complementary filter
  */
void Get_Complementary_Roll_Pitch(float angles[2], float acc_angles[2], float angle_change[3], float dt, float alpha){
  angles[0] += degToRad(angle_change[0]) * dt;   
  angles[1] += degToRad(angle_change[1]) * dt;   

  angles[0] = alpha * acc_angles[0] + (1-alpha) * angles[0];
  angles[1] = alpha * acc_angles[1] + (1-alpha) * angles[1];
}

void Get_XY_Velocities(float acc[3], float angles[3]){
  (void)acc;
  (void)angles;
}

/**
 * @brief one dimensional Kalman filter
  */
void Kalman_init(kalman_t *kalman){
  kalman->kalman_extrapolation_term = kalman->sampling_time * kalman->sampling_time * kalman->angular_velocity_variance;
  
  /* initial guess */
  kalman->variance_prediction = sqrt(kalman->angle_variance);
}

void Kalman_calculate(kalman_t *kalman, float *kalman_state, float measurement, float velocity){
  /* predict current state */
  *kalman_state = *kalman_state + kalman->sampling_time * velocity;
  /* calculate current variance */
  kalman->variance_prediction = kalman->variance_prediction + kalman->kalman_extrapolation_term; 
  /* update kalman gain */
  kalman->kalman_gain = kalman->variance_prediction/(kalman->variance_prediction + kalman->angle_variance);
  /* predict kalman angle */
  *kalman_state = *kalman_state + kalman->kalman_gain * (measurement - *kalman_state);
  /* update variance */
  kalman->variance_prediction = (1 - kalman->kalman_gain) * kalman->variance_prediction;
}

#ifdef KALMAN
static kalman_t kalman_pitch;
static kalman_t kalman_roll;
#endif
static IIR_filter_t iir;
static float s_dt, s_alpha;

/**
 * @brief Initialize Angle estimator
 * 
 * @param dt 
 * @param alpha 
 * @param tau 
 */
void Estimate_Angles_Init(float dt, float alpha, float tau){
  s_dt = dt;
  s_alpha = alpha;

  iir.samplingTime = dt;
  iir.tau = tau;
  Low_Pass_IIR_Filter_Init(&iir);

  #ifdef KALMAN
  kalman_pitch.sampling_time = dt;
  kalman_pitch.angular_velocity_variance = degToRad(4) * degToRad(4);
  kalman_pitch.angle_variance = degToRad(3) * degToRad(3);
  Kalman_init(&kalman_pitch);

  kalman_roll.sampling_time = dt;
  kalman_roll.angular_velocity_variance = degToRad(4) * degToRad(4);
  kalman_roll.angle_variance = degToRad(3) * degToRad(3);
  Kalman_init(&kalman_roll);  
  #endif
}

/**
 * @brief Calculates Euler angles estimates using 
 *        1D Kalman or complementary filter
 * 
  */
void Estimate_Angles(float angles[2], float angular_velocities[3], const float acc_buf[3], const float gyro_buff[3]){
  const int x = 0, y = 1, z = 2;
  float acc_angles[2];
  float filtered_acc[3];

  static float filter_acc_x_in[2]  = {0};
  static float filter_acc_x_out[2] = {0};

  static float filter_acc_y_in[2]  = {0};
  static float filter_acc_y_out[2] = {0};

  static float filter_acc_z_in[2]  = {0};
  static float filter_acc_z_out[2] = {0};

  filter_acc_x_in[0] = acc_buf[x];
  Low_Pass_IIR_Filter(&iir, filter_acc_x_out, filter_acc_x_in);
  filtered_acc[x] = filter_acc_x_out[0];

  filter_acc_y_in[0] = acc_buf[y];
  Low_Pass_IIR_Filter(&iir, filter_acc_y_out, filter_acc_y_in);
  filtered_acc[y] = filter_acc_y_out[0];

  filter_acc_z_in[0] = acc_buf[z];
  Low_Pass_IIR_Filter(&iir, filter_acc_z_out, filter_acc_z_in);
  filtered_acc[z] = filter_acc_z_out[0];

  Calculate_Angles_acc(filtered_acc, acc_angles);
  Calculate_Angular_Velocities(angular_velocities, angles, gyro_buff);

  angular_velocities[2] = roundf(angular_velocities[2] / 10.0f);

  #ifdef KALMAN
  Kalman_calculate(&kalman_pitch, &angles[0], acc_angles[0], degToRad(angular_velocities[0]));
  Kalman_calculate(&kalman_roll, &angles[1], acc_angles[1], degToRad(angular_velocities[1]));
  #else
  angles[0] += degToRad(angular_velocities[0]) * s_dt;   
  angles[1] += degToRad(angular_velocities[1]) * s_dt;   

  angles[0] = s_alpha * acc_angles[0] + (1-s_alpha) * angles[0];
  angles[1] = s_alpha * acc_angles[1] + (1-s_alpha) * angles[1];  
  #endif // KALMAN
}
