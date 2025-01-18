# Add opticalflow library
add_library(opticalflow STATIC
    hal_opticalflow.c
    serial_protocol.c
    serial_protocol_packets.c
    serial_protocol_ring_buffer.c
)

# Add linked libraries
target_link_libraries(opticalflow PRIVATE ${MCU_FAMILY_LOWER}Hal)

# Add include directories
target_include_directories(opticalflow
    PRIVATE
        ${CMAKE_SOURCE_DIR}/HAL_Flight_Controller/OPTICALFLOW
    PUBLIC
        ${CMAKE_SOURCE_DIR}/HAL_Flight_Controller
)
