cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

include(config.cmake)

# Set the linker script
if(USE_BOOTLOADER)
    # Set the vector table start address
    set(USER_VECT_TAB_ADDRESS ON)
    set(LINKER_SCRIPT_NAME ${MCU_MODEL}_FLASH_BOOTLOADER.ld)
else()
    set(LINKER_SCRIPT_NAME ${MCU_MODEL}_FLASH.ld)
endif()

string(TOLOWER ${MCU_MODEL} MCU_MODEL_LOWER)
string(TOLOWER ${MCU_FAMILY} MCU_FAMILY_LOWER)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME fc-stm32)

# Include toolchain file
include(gcc-arm-none-eabi.cmake)

# Include functions file
include(functions.cmake)

# Include external projects
include(external.cmake)

# Include requirements
include(requirements.cmake)
FetchContent_MakeAvailable(serial_cli)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
    # Core/Src/drivers/bmp280.c
    # Core/Src/drivers/flash_memory.c
    # Core/Src/drivers/ibus.c
    Core/Src/drivers/motors.c
    Core/Src/dsp/angle_estimation.c
    Core/Src/dsp/filters.c
    Core/Src/dsp/pid.c
    Core/Src/dsp/stabilizer.c
    
    Core/Src/flight_controller.c
    Core/Src/logger.c
    Core/Src/rc.c
    Core/Src/cli_commands.c
    Core/Src/main.c

    Core/Src/syscalls.c
    Core/Src/sysmem.c

    HAL_STM32/${MCU_FAMILY}/Src/${MCU_FAMILY_LOWER}_hal_msp.c
    HAL_STM32/${MCU_FAMILY}/Src/${MCU_FAMILY_LOWER}_it.c
    HAL_STM32/${MCU_FAMILY}/Src/system_${MCU_FAMILY_LOWER}.c
    HAL_STM32/${MCU_FAMILY}/startup_${MCU_MODEL_LOWER}.s
)

include_directories(
    Core/Inc
    Core/Inc/drivers
    Core/Inc/dsp
)

# Add STM32CubeMX generated sources
add_subdirectory(HAL_STM32/${MCU_FAMILY}_HAL_Driver)

# Add selected libraries
add_subdirectory(HAL_Flight_Controller/MPU6050)
add_subdirectory(HAL_Flight_Controller/NRF24L01)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${MCU_FAMILY_LOWER}Hal
    mpu6050
    nrf24l01
    serial_cli
)

# Create Intel HEX file
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMENT "Generating HEX file"
)

target_stlink_flash_app(${CMAKE_PROJECT_NAME})
target_stlink_flash_bootloader("STM32f103_msd_bootloader")