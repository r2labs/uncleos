cmake_minimum_required(VERSION 2.8.7)

# TODO: Why can't I put this in the toolchain file
set(CMAKE_CXX_FLAGS "-fno-rtti -fno-exceptions -MD -Wall -ffunction-sections -fdata-sections -Dgcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -DPART_TM4C123GH6PM -DARM_MATH_CM4 -D__FPU_PRESENT=0")

set (unclelib-src blinker.cpp timerpp.cpp uartpp.cpp shellpp.cpp
  semaphorepp.cpp canpp.cpp adcpp.cpp motorpp.cpp drivepp.cpp ir.cpp
  pingpp.cpp math.cpp ctlsysctl.cpp)

include_directories(../)

add_library (unclelib STATIC ${unclelib-src})
