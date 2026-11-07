cmake_minimum_required(VERSION 3.13)


set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.3.0")

include(pico_sdk_import.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(rocket-controller)

pico_sdk_init()

add_library(drivers STATIC
    src/drivers/radio.cpp
    src/drivers/gps.cpp
)
target_include_directories(drivers PUBLIC src)
target_link_libraries(drivers PUBLIC pico_stdlib_headers hardware_spi_headers)

add_library(flight-board STATIC
    src/board/flight-controller/board.cpp
)
add_library(receiver-board STATIC
    src/board/radio-receiver/board.cpp
)
target_link_libraries(flight-board PRIVATE drivers pico_stdlib)
target_link_libraries(receiver-board PRIVATE drivers pico_stdlib)

add_executable(rocket-controller src/controllers/flight-controller.cpp)
target_link_libraries(rocket-controller flight-board drivers pico_stdlib hardware_spi)
target_compile_options(rocket-controller PRIVATE -Wall -Wextra)

add_executable(radio-receiver src/controllers/radio-receiver.cpp)
target_link_libraries(radio-receiver receiver-board drivers pico_stdlib hardware_spi)
target_compile_options(radio-receiver PRIVATE -Wall -Wextra)

pico_enable_stdio_usb(rocket-controller 1)
pico_enable_stdio_uart(rocket-controller 0)
pico_add_extra_outputs(rocket-controller)

pico_enable_stdio_usb(radio-receiver 1)
pico_enable_stdio_uart(radio-receiver 0)
pico_add_extra_outputs(radio-receiver)
