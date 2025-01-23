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
    Src/stm32f1xx_hal_pcd.c
    Src/stm32f1xx_hal_pcd_ex.c
    Src/stm32f1xx_ll_usb.c

    ../STM32F1xx/Src/dma.c
    ../STM32F1xx/Src/gpio.c
    ../STM32F1xx/Src/i2c.c
    ../STM32F1xx/Src/spi.c
    ../STM32F1xx/Src/tim.c
    ../STM32F1xx/Src/usart.c

    ../STM32F1xx/USB_DEVICE/App/usb_device.c
    ../STM32F1xx/USB_DEVICE/App/usbd_cdc_if.c
    ../STM32F1xx/USB_DEVICE/App/usbd_desc.c
    ../STM32F1xx/USB_DEVICE/Target/usbd_conf.c

    ../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_conf_template.c
    ../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    ../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    ../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_desc_template.c
    ../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    ../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
    ../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc_if_template.c
)

target_compile_definitions(stm32f1xxHal PUBLIC 
	USE_HAL_DRIVER 
	STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

target_compile_options(stm32f1xxHal PRIVATE -Wno-unused-parameter)

target_include_directories(stm32f1xxHal PUBLIC
    Inc
    Inc/Legacy
    ../STM32F1xx/Inc
    ../STM32F1xx/USB_DEVICE/App
    ../STM32F1xx/USB_DEVICE/Target
    ../Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ../CMSIS/Device/ST/STM32F1xx/Include
    ../CMSIS/Include
)