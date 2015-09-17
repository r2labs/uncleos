cmake_minimum_required(VERSION 2.8.7)

project(test-countdown)
set(test-countdown-srcs ../startup_gcc_canonical.c test-countdown.cpp)

include_directories(../../)

add_executable(test-countdown ${test-countdown-srcs})
target_link_libraries(test-countdown driverlib)
target_link_libraries(test-countdown unclelib)
target_link_libraries(test-countdown uncleos)
