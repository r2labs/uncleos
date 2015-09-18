cmake_minimum_required(VERSION 2.8.7)

# TODO: Why is this here? What does it do? Why is it required?
set(CMAKE_C_COMPILE_FLAGS "-g -Wall -ffunction-sections -fdata-sections -Dgcc")

# TODO: Why can't I put this in the toolchain file
set(CMAKE_CXX_FLAGS "-fno-rtti -fno-exceptions -MD -mcpu=cortex-m4 -march=armv7e-m -mthumb -DPART_TM4C123GH6PM -DARM_MATH_CM4 -D__FPU_PRESENT=0")

set(CMAKE_EXE_LINKER_FLAGS "-T ../map.ld --entry Reset_Handler -nostdlib")
configure_file(map.ld map.ld COPYONLY)

add_subdirectory(test-countdown)
