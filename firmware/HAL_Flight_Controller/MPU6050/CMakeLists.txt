add_library(mpu6050
    mpu6050.c
    hal_imu.c
)

# Add linked libraries
target_link_libraries(mpu6050 PRIVATE ${MCU_FAMILY_LOWER}Hal)

# Add include directories
target_include_directories(mpu6050
    PRIVATE
        ${CMAKE_SOURCE_DIR}/HAL_Flight_Controller/MPU6050
    PUBLIC
        ${CMAKE_SOURCE_DIR}/HAL_Flight_Controller
)