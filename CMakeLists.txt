cmake_minimum_required(VERSION 3.13)


set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.3.0")

include(pico_sdk_import.cmake)

project(rocket-controller)

pico_sdk_init()

add_executable(rocket-controller src/main.cpp)

target_link_libraries(rocket-controller pico_stdlib hardware_spi)

pico_enable_stdio_usb(rocket-controller 1)
pico_enable_stdio_uart(rocket-controller 0)


 

pico_add_extra_outputs(rocket-controller)
