# Add nrf24l01 library
add_library(nrf24l01
    nrf24l01.c
    hal_radio.c
)

# Add linked libraries
target_link_libraries(nrf24l01 PRIVATE ${MCU_FAMILY_LOWER}Hal)

# Add include directories
target_include_directories(nrf24l01
    PRIVATE
        ${CMAKE_SOURCE_DIR}/HAL_Flight_Controller/NRF24L01
    PUBLIC
        ${CMAKE_SOURCE_DIR}/HAL_Flight_Controller
)
