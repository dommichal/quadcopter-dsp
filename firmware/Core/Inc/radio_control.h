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

enum {
    THRUST = 0,
    PITCH = 1,
    YAW = 2,
    ROLL = 3
};

typedef union RadioTelemtery {
    float floatingPoint[6];
    uint8_t bytes[24];
} RadioTelemtery;

typedef struct RadioControl {
    int8_t controls_inputs[4];
    bool power_on;
} RadioControl;

void RadioControl_ReceiveMessage(const uint8_t message[8], RadioControl *rc);
void RadioControl_ConnectionTick();
bool RadioControl_CheckConnection();
void RadioControl_DecreaseAltitude(RadioControl *rc);

#endif // RC
