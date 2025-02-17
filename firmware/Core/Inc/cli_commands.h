#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

#include "serial_cli.h"

#ifdef __cplusplus
extern "C" {
#endif

void SerialCLI_RegisterAllCommands(SerialCLI *cli);

#ifdef __cplusplus
}
#endif

#endif // CLI_COMMANDS_H