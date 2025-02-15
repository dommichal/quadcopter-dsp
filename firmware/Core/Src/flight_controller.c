#include "flight_controller.h"

#include "dsp/stabilizer.h"
#include "dsp/angle_estimation.h"

#include "serial_cli.h"
#include "cli_commands.h"
#include "radio_control.h"
#include "nvm.h"
#include "serial_cli.h"

#include "usbd_cdc_if.h"
#include "main.h"

static RadioControl rc;
static RadioTelemtery telemetry;
static SerialCLI cli;
static NVM_Storage *storage;

void CDC_ReceiveCallBack(uint8_t *Buf, uint32_t Len) {
    SerialCLI_Read(&cli, (char *)Buf, Len);
}

static void cliWrite(const char *str, size_t len) {
    uint32_t retries = 0;
    while (CDC_Transmit_FS((uint8_t *)str, len) != USBD_OK) {
        if (retries++ > 1000) {
            break;
        }
    }
}

void HAL_RADIO_request_receive_callback(void) {
    HAL_RADIO_write_telemetry_payload(telemetry.bytes, 24);
}   

void HAL_RADIO_receive_complete_callback(const uint8_t *packet,
                                         uint8_t packet_length) {
    (void)packet_length;
    RadioControl_ConnectionTick(&rc);
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    RadioControl_ReceiveMessage(packet, &rc);
}

void IMU_conversion_complete_callback(const float *acc, const float *gyro) {
    static float angle_change[3];
    static float angles[3];

    Estimator_DetermineAngles(angles, angle_change, acc, gyro);
    Stabilizer_Update(angles, angle_change, rc.controls_inputs);
    Motors_Switch(rc.power_on);

    telemetry.floatingPoint[0] = radToDeg(angles[0]);
    telemetry.floatingPoint[1] = radToDeg(angles[1]);
    telemetry.floatingPoint[2] = (float)rc.controls_inputs[THRUST];
    telemetry.floatingPoint[3] = (float)rc.controls_inputs[PITCH];
}

void FC_init() {
    NVM_Init();
    storage = NVM_GetStorage();

    Stabilizer_init();
    const float dt = 0.001f, comp_alpha = 0.001f, iir_tau = 0.04f;
    Estimator_Init(dt, comp_alpha, iir_tau);

    HAL_RADIO_init(HAL_RADIO_receive_complete_callback,
                   HAL_RADIO_request_receive_callback);
    HAL_IMU_init(IMU_conversion_complete_callback, &storage->imuCalibration);

    SerialCLI_Init(&cli, &cliWrite);
    SerialCLI_RegisterAllCommands(&cli);

    HAL_RADIO_start_listening();
    HAL_IMU_start_conversion();
}

void FC_deinit() {
    HAL_RADIO_deinit();
    HAL_IMU_deinit();
}

void FC_proc() {
    if (RadioControl_CheckConnection()) {
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
        RadioControl_DecreaseAltitude(&rc);
        
        SerialCLI_Process(&cli);
    }
}