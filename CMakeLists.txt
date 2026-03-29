cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pulse_looper_card)

pico_sdk_init()

add_executable(pulse_looper_card
    PulseLooper.cpp
)

target_include_directories(pulse_looper_card PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ComputerCard
)

target_link_libraries(pulse_looper_card
    pico_stdlib
    hardware_adc
    hardware_gpio
    hardware_timer
    hardware_pwm
    hardware_dma
    hardware_flash
    hardware_i2c
    hardware_spi
)

pico_add_extra_outputs(pulse_looper_card)