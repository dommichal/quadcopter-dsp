#include "flight_controller.h"
#include "main.h"

static RC_t rc;
static Telemetry_t telemetry;

void HAL_RADIO_request_receive_callback(void) {
    HAL_RADIO_write_telemetry_payload(telemetry.bytes, 24);
}

void HAL_RADIO_receive_complete_callback(const uint8_t *packet,
                                         uint8_t packet_length) {
    (void)packet_length;
    RC_Connection_Tick(&rc);
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    RC_Receive_Message(packet, &rc);
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
    // telemetry.floatingPoint[4] = (float)rc.controls_inputs[yaw];
    // telemetry.floatingPoint[5] = (float)rc.controls_inputs[roll];
}

void FC_init() {
    Stabilizer_init();
    const float dt = 0.001f, comp_alpha = 0.001f, iir_tau = 0.04f;
    Estimate_Angles_Init(dt, comp_alpha, iir_tau);

    HAL_RADIO_init(HAL_RADIO_receive_complete_callback,
                   HAL_RADIO_request_receive_callback);
    HAL_IMU_init(IMU_conversion_complete_callback);
    HAL_IMU_calibrate();
    
    HAL_IMU_start_conversion();
    HAL_RADIO_start_listening();
}

void FC_deinit() {
    HAL_RADIO_deinit();
    HAL_IMU_deinit();
}

void FC_proc() {
    if (RC_Check_Connection()) {
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
        Lower_Altitude(&rc);
    }
}