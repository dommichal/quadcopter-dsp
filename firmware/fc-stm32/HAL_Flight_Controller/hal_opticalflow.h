#ifndef HAL_OPTICALFLOW_H_
#define HAL_OPTICALFLOW_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_OPTICALFLOW_INTERFACE_UART

typedef uint32_t HAL_OPTICALFLOW_timepoint_t;

typedef struct {
    float yaw;
    float pitch;
    float roll;
    float thrust;
} HAL_OPTICALFLOW_conversion_result_t;

typedef void (*HAL_OPTICALFLOW_conversion_complete_callback_t)(const HAL_OPTICALFLOW_conversion_result_t *result);

void HAL_OPTICALFLOW_init(HAL_OPTICALFLOW_conversion_complete_callback_t conversion_complete_callback);
void HAL_OPTICALFLOW_deinit(void);
void HAL_OPTICALFLOW_calculate(int16_t velocities[3], float angles[3], float angular_rates[3], HAL_OPTICALFLOW_timepoint_t now);
void HAL_OPTICALFLOW_start_conversion(void);
void HAL_OPTICALFLOW_readout(void);

#ifdef __cplusplus
}
#endif

#endif // HAL_OPTICALFLOW_H_