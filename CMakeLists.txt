cmake_minimum_required(VERSION 3.13)


set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.3.0")

include(pico_sdk_import.cmake)

project(hab-flight)

pico_sdk_init()

add_executable(hab-flight src/main.cpp)

target_link_libraries(hab-flight pico_stdlib)

pico_add_extra_outputs(hab-flight)
