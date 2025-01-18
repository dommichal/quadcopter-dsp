#if !defined(RC)
#define RC
#include <main.h>
#include <stdbool.h>
#include <stdint.h>

#define ACTIVATION_THRESHOLD 3
#define YAW_ACTIVATION_THRESHOLD 10
#define ACTIVATION_THRESHOLD_THRUST 10

#define ROLL_ANGLE_SCALE 0.15f
#define PITCH_ANGLE_SCALE 0.15f
#define YAW_ANGLE_SCALE 1.00f

#define MAX_CONTROLLER_TIMEOUT 700

typedef enum Control_inputs_t {
    thrust = 0,
    pitch = 1,
    yaw = 2,
    roll = 3
} Control_Inputs_t;

typedef union Telemetry_t {
    float floatingPoint[6];
    uint8_t bytes[24];
} Telemetry_t;

typedef struct RC_t {
    int8_t controls_inputs[4];
    bool power_on;
} RC_t;

void RC_Receive_Message(const uint8_t message[8], RC_t *rc);
void RC_Connection_Tick();
bool RC_Check_Connection();
void Lower_Altitude(RC_t *rc);

#endif // RC
