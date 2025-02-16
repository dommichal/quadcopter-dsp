#include "cli_commands.h"

#include <stdint.h>

#include "hal_imu.h"
#include "usb_device.h"
#include "nvm.h"

void firmwareUpdateCommand(SerialCLI *cli, int argc, const char **argv) {
    (void)argv;
    (void)argc;

    SerialCLI_WriteString(cli, "Firmware update started.\r\n");

    MX_USB_DEVICE_DeInit();
    USB_DevDisconnect(USB); // disconnect USB device

    /* Pull USB D+ PIN to GND so USB Host detects device disconnect */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_LOW);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_12, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_12);
    HAL_Delay(1000);

    // Store bootkey in RAM and reset
    // to force the bootloader to start
    *((volatile uint32_t *)0x20004ffc) = 0x12345678; // same BOOTKEY as defined in Bootloader

    NVIC_SystemReset();
}

void calibrateCommand(SerialCLI *cli, int argc, const char **argv) {
    (void)argv;
    (void)argc;

    SerialCLI_WriteString(cli, "Calibrating IMU...\r\n");

    NVM_Storage *storage = NVM_GetStorage();
    HAL_IMU_Calibration *calibration =  &storage->imuCalibration;
    
    if (NULL == storage) {
        return;
    }

    HAL_IMU_calibrate(calibration);

    if (NVM_SaveData(storage)) {
        SerialCLI_WriteString(cli, "Calibration saved.\r\n");
    } else {
        SerialCLI_WriteString(cli, "Failed.\r\n");
    }
}

void trimCommand(SerialCLI *cli, int argc, const char **argv) {
    NVM_Storage *storage = NVM_GetStorage();
    if (NULL == storage) {
        return;
    }

    if (argc < 3) {
        SerialCLI_WriteString(cli, "Usage: trim <roll> <pitch>\r\n");
        SerialCLI_WriteString(cli, "Current settings: roll %d, pitch %d\r\n", storage->trim.roll, storage->trim.pitch);
        return;
    }

    storage->trim.roll = atoi(argv[1]);
    storage->trim.pitch = atoi(argv[2]);

    if (NVM_SaveData(storage)) {
        SerialCLI_WriteString(cli, "Saved: roll %d, pitch %d\r\n", storage->trim.roll, storage->trim.pitch);
    }
}

SerialCLI_CommandEntry commands[] = {
    {firmwareUpdateCommand, "update", "DFU in mass storage mode."},
    {calibrateCommand, "calibrate", "Calibrate IMU sensors."},
    {trimCommand, "trim", "Set trim of roll and pitch angles."},
};

enum {
    CLI_COMMANDS_COUNT = sizeof(commands) / sizeof(commands[0]),
};

void SerialCLI_RegisterAllCommands(SerialCLI *cli) {
    for (size_t i = 0; i < CLI_COMMANDS_COUNT; ++i) {
        SerialCLI_RegisterCommand(cli, &commands[i]);
    }
}