#include "cli_commands.h"

#include <stdint.h>

#include "hal_imu.h"
#include "usb_device.h"

void firmwareUpdateCommand(SerialCLI *cli, int argc, const char **argv) {
    (void)argv;
    if (1 != argc) {
        SerialCLI_WriteString(cli, "Usage: firmware-update\r\n");
        return;
    }
    SerialCLI_WriteString(cli, "Firmware update mode started.\r\n");

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
    HAL_FLASH_Unlock();
    *((volatile uint32_t *)0x20004ffc) = 0x12345678; // same BOOTKEY as defined in Bootloader
    HAL_FLASH_Lock();

    NVIC_SystemReset();
}

void calibrateCommand(SerialCLI *cli, int argc, const char **argv) {
    (void)argv;
    if (1 == argc) {
        SerialCLI_WriteString(cli, "Calibrating IMU...\r\n");
        HAL_IMU_calibrate();
        SerialCLI_WriteString(cli, "Calibration complete.\r\n");
    } else {
        SerialCLI_WriteString(cli, "Usage: calibrate\r\n");
    }
}

SerialCLI_CommandEntry commands[] = {
    {firmwareUpdateCommand, "update", "Open the mass storage device for firmware update."},
    {calibrateCommand, "calibrate", "Calibrate the IMU."},
};

enum {
    CLI_COMMANDS_COUNT = sizeof(commands) / sizeof(commands[0]),
};

void SerialCLI_RegisterAllCommands(SerialCLI *cli) {
    for (size_t i = 0; i < CLI_COMMANDS_COUNT; ++i) {
        SerialCLI_RegisterCommand(cli, &commands[i]);
    }
}