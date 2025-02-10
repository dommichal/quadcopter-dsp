#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_imu.h"
#include "hal_radio.h"

#include "dsp/stabilizer.h"
#include "dsp/angle_estimation.h"

#include "rc.h"
#include "serial_cli.h"

void FC_init(void);
void FC_proc(void);

#ifdef __cplusplus
}
#endif

#endif // FLIGHT_CONTROLLER_H