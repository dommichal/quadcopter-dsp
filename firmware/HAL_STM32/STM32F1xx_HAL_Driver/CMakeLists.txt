add_library(stm32f1xxHal STATIC    
    Src/stm32f1xx_hal_gpio_ex.c
    Src/stm32f1xx_hal_i2c.c
    Src/stm32f1xx_hal.c
    Src/stm32f1xx_hal_rcc.c
    Src/stm32f1xx_hal_rcc_ex.c
    Src/stm32f1xx_hal_gpio.c
    Src/stm32f1xx_hal_dma.c
    Src/stm32f1xx_hal_cortex.c
    Src/stm32f1xx_hal_pwr.c
    Src/stm32f1xx_hal_flash.c
    Src/stm32f1xx_hal_flash_ex.c
    Src/stm32f1xx_hal_exti.c
    Src/stm32f1xx_hal_spi.c
    Src/stm32f1xx_hal_tim.c
    Src/stm32f1xx_hal_tim_ex.c
    Src/stm32f1xx_hal_uart.c

    ../STM32F1xx/Src/dma.c
    ../STM32F1xx/Src/gpio.c
    ../STM32F1xx/Src/i2c.c
    ../STM32F1xx/Src/spi.c
    ../STM32F1xx/Src/tim.c
    ../STM32F1xx/Src/usart.c
)

target_compile_definitions(stm32f1xxHal PUBLIC 
	USE_HAL_DRIVER 
	STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32f1xxHal PUBLIC
    Inc
    Inc/Legacy
    ../STM32F1xx/Inc
    ../CMSIS/Device/ST/STM32F1xx/Include
    ../CMSIS/Include
)