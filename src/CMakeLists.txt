add_executable(msboot_pico) 

target_sources(msboot_pico 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/configuration.hpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/configuration.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/connection.hpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/connection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware_loader.hpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/firmware_loader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/functions.s
)

find_package(eul REQUIRED)
find_package(littlefs REQUIRED)

target_link_libraries(msboot_pico
    PRIVATE 
        hardware_flash
        pico_stdlib
        eul_crc
        littlefs
)

#pico_set_linker_script(msboot_pico 
#    ${PROJECT_SOURCE_DIR}/linker/bootloader.ld 
#)

pico_enable_stdio_usb(msboot_pico 0)
pico_enable_stdio_uart(msboot_pico 1)

pico_add_extra_outputs(msboot_pico)
