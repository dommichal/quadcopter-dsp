#if !defined(ANGLE_ESTIMATION)
#define ANGLE_ESTIMATION
#include "math.h"
#include "stdbool.h"
#include <filters.h>

#define degToRad(angleInDegrees) ((angleInDegrees) * 3.141592f / 180.0f)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0f / 3.141592f)

#define AX_OFFSET   0.05f

#define KALMAN

typedef struct kalman_t
{
    float kalman_angle;
    float kalman_gain;
    float variance_prediction;    
    float kalman_extrapolation_term;

    float sampling_time;
    float angular_velocity_variance;
    float angle_variance;
}kalman_t;


void Calculate_Angles_acc(float acc_buf[3], float angles[2]);
void Calculate_Angular_Velocities(float angle_change[3], float angles[2], const float gyro[3]);
void Get_Complementary_Roll_Pitch(float angles[2], float acc_angles[2], float angle_change[3], float dt, float alpha);

void Kalman_init(kalman_t *kalman);
void Kalman_calculate(kalman_t *kalman, float *kalman_state, float measurement, float velocity);

void Estimate_Angles_Init(float dt, float alpha, float tau);
void Estimate_Angles(float angles[2], float angular_velocities[3], const float acc_buf[3], const float gyro_buf[3]);

#endif // ANGLE_ESTIMATION
