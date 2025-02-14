#include "flight_controller.h"

#include "dsp/stabilizer.h"
#include "dsp/angle_estimation.h"

#include "serial_cli.h"
// #include "flash_memory.h"
#include "cli_commands.h"
#include "radio_control.h"
#include "serial_cli.h"

#include "usbd_cdc_if.h"
#include "main.h"

static RadioControl rc;
static RadioTelemtery telemetry;
static SerialCLI cli;

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

    Estimate_Angles(angles, angle_change, acc, gyro);
    Stabilize(angles, angle_change, rc.controls_inputs);
    Motors_Switch(rc.power_on);

    telemetry.floatingPoint[0] = radToDeg(angles[0]);
    telemetry.floatingPoint[1] = radToDeg(angles[1]);
    telemetry.floatingPoint[2] = (float)rc.controls_inputs[thrust];
    telemetry.floatingPoint[3] = (float)rc.controls_inputs[pitch];
}

void FC_init() {
    Stabilizer_init();
    const float dt = 0.001f, comp_alpha = 0.001f, iir_tau = 0.04f;
    Estimate_Angles_Init(dt, comp_alpha, iir_tau);

    HAL_RADIO_init(HAL_RADIO_receive_complete_callback,
                   HAL_RADIO_request_receive_callback);
    HAL_IMU_init(IMU_conversion_complete_callback);

    SerialCLI_Init(&cli, &cliWrite);
    SerialCLI_RegisterAllCommands(&cli);

    // uint32_t test = 0xFE;
    // FlashMemory_SaveData(0x08007C00U, &test, 1);

    // uint32_t read_test;
    // FlashMemory_ReadData(0x08007C00U, &read_test, 1);

    HAL_Delay(1000);

    // HAL_IMU_calibrate();
    HAL_RADIO_start_listening();
    HAL_IMU_start_conversion();
}

void FC_deinit() {
    HAL_RADIO_deinit();
    HAL_IMU_deinit();
}

void FC_proc() {
    SerialCLI_Process(&cli);
    if (RadioControl_CheckConnection()) {
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
        RadioControl_DecreaseAltitude(&rc);
    }
}