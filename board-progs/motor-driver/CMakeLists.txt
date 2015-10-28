cmake_minimum_required(VERSION 2.8.7)

project(motor-driver)
set(motor-driver-srcs ../startup_gcc_canonical.c motor-driver.cpp)

include_directories(../../)

add_executable(motor-driver ${motor-driver-srcs})
target_link_libraries(motor-driver driverlib)
target_link_libraries(motor-driver unclelib)
target_link_libraries(motor-driver uncleos)
target_link_libraries(motor-driver /usr/lib/gcc/arm-none-eabi/5.2.0/thumb/libgcc.a)
target_link_libraries(motor-driver /usr/arm-none-eabi/lib/thumb/libc.a)
