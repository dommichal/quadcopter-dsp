include(ExternalProject)

ExternalProject_Add(
    MSD_Bootloader
    GIT_REPOSITORY https://github.com/Cyberdom123/STM32F103_MSD_BOOTLOADER.git
    GIT_TAG        master
    INSTALL_COMMAND ""  # Disable the install step    
)