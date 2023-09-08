# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "d:/RaspberryPiPico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(lights C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1


add_executable(lights main.c )

pico_set_program_name(lights "lights")
pico_set_program_version(lights "0.2")

pico_enable_stdio_uart(lights 1)
pico_enable_stdio_usb(lights 1)

target_sources(lights PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/loop.c
        ${CMAKE_CURRENT_LIST_DIR}/blinker.c
        ${CMAKE_CURRENT_LIST_DIR}/spi_lights.c
        )

# Add the standard library to the build
target_link_libraries(lights pico_stdlib)

# Add any user requested libraries
target_link_libraries(lights
        hardware_timer
        hardware_watchdog
        hardware_clocks
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(lights pico_stdlib hardware_spi)

# pico_add_extra_outputs(main)
# must be removed or build fails
