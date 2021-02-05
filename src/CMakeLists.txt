add_executable(msboot_pico) 

target_sources(msboot_pico 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp 
)


target_link_libraries(msboot_pico
    PRIVATE 
        hardware_flash
        pico_stdlib 
)

pico_enable_stdio_usb(msboot_pico 1)
pico_enable_stdio_uart(msboot_pico 0)

pico_add_extra_outputs(msboot_pico)
