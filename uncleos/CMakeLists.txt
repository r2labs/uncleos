cmake_minimum_required(VERSION 2.8.7)

set (uncleos-src os.c)
include_directories(../)

add_library (uncleos STATIC ${uncleos-src})

target_link_libraries(uncleos driverlib)
