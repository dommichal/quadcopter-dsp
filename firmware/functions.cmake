function(target_stlink_flash_app TARGET)
    add_custom_target(${TARGET}.flash_stlink
            openocd 
            -f ${CMAKE_SOURCE_DIR}/Devices/Debug/openocd.cfg
            -c "program ${CMAKE_SOURCE_DIR}/build/${TARGET}.elf verify reset exit"
            DEPENDS ${TARGET}
            COMMENT "Flashing target hardware"
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endfunction(target_stlink_flash_app)

function(target_stlink_flash_bootloader TARGET)
    add_custom_target(bootloader.flash_stlink
            openocd 
            -f ${CMAKE_SOURCE_DIR}/Devices/Debug/openocd.cfg
            -c "program ${CMAKE_SOURCE_DIR}/build/MSD_Bootloader-prefix/src/MSD_Bootloader-build/${TARGET}.elf reset exit"
            COMMENT "Flashing target hardware"
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endfunction(target_stlink_flash_bootloader)
