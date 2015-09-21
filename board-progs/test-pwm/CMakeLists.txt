cmake_minimum_required(VERSION 2.8.7)

project(test-pwm)
set(test-pwm-srcs ../startup_gcc_canonical.c test-pwm.cpp)

include_directories(../../)

add_executable(test-pwm ${test-pwm-srcs})
target_link_libraries(test-pwm driverlib)
target_link_libraries(test-pwm unclelib)
target_link_libraries(test-pwm uncleos)
