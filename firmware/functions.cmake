function(target_stlink_flash CMAKE_PROJECT_NAME)
    add_custom_target(flash_${CMAKE_PROJECT_NAME}
        openocd
        -f ${CMAKE_SOURCE_DIR}/Devices/Debug/openocd.cfg
        -c "program ${BUILD_DIR}/${CMAKE_PROJECT_NAME}.elf verify reset exit"
        COMMENT "Flashing ${CMAKE_PROJECT_NAME} with OpenOCD"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    add_dependencies(flash flash_${CMAKE_PROJECT_NAME})
endfunction(target_stlink_flash)


function(target_stlink_flash TARGET)
    add_custom_target(${TARGET}.flash_stlink
            openocd 
            -f ${CMAKE_SOURCE_DIR}/Devices/Debug/openocd.cfg
            -c "program ${CMAKE_SOURCE_DIR}/build/${TARGET}.elf verify reset exit"
            DEPENDS ${TARGET}
            COMMENT "Flashing target hardware"
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endfunction(target_stlink_flash)