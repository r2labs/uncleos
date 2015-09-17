cmake_minimum_required(VERSION 2.8.7)

set (unclelib-src blinker.cpp timerpp.cpp uartpp.cpp shellpp.cpp
  semaphorepp.cpp canpp.cpp adcpp.cpp motorpp.cpp drivepp.cpp ir.cpp
  pingpp.cpp math.cpp ctlsysctl.cpp)

include_directories(../)

add_library (unclelib STATIC ${unclelib-src})
