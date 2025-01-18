#if !defined(PID)
#define PID

typedef struct {
    /* constants */
    float kp;
    float ki;
    float kd;
    float sampleTime;

    /* errors */
    float IntError;
    float lastError;
    float derivError;
    float lastDeriv;

    /* filter */
    float tau;
    float lowPassTerm;

    /* limits */
    float maxOut;
    float minOut;
    float maxInt;
    float minInt;

} pid_t;

/**
 * @brief Initializes the PID controller.
 *
 * This function initializes the PID controller by setting the initial values of the PID parameters.
 *
 * @param pid Pointer to the PID controller structure.
 */
void PID_init(pid_t *pid);

/**
 * @brief Calculates the output of a PID controller.
 *
 * This function calculates the output of a PID controller based on the provided input and target values.
 *
 * @param pid Pointer to the PID controller structure.
 * @param input The current input value.
 * @param target The desired target value.
 * @return The calculated output of the PID controller.
 */
float PID_Calculate(pid_t *pid, float input, float target);

#endif // PID
