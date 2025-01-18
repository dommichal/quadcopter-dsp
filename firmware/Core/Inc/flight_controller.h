#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_imu.h"
#include "hal_radio.h"

#include "rc.h"
#include "dsp/stabilizer.h"
#include "dsp/angle_estimation.h"

void FC_init(void);
void FC_proc(void);

#ifdef __cplusplus
}
#endif

#endif // FLIGHT_CONTROLLER_H