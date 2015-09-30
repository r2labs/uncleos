cmake_minimum_required(VERSION 2.8.7)

project(test-cpp)
set(test-cpp-srcs ../startup_gcc_canonical.c tinynew.cpp test-cpp.cpp)

include_directories(../../)

add_executable(test-cpp ${test-cpp-srcs})
target_link_libraries(test-cpp driverlib)
target_link_libraries(test-cpp unclelib)
target_link_libraries(test-cpp uncleos)
target_link_libraries(test-cpp /usr/lib/gcc/arm-none-eabi/5.2.0/thumb/libgcc.a)
target_link_libraries(test-cpp /usr/arm-none-eabi/lib/thumb/libc.a)
