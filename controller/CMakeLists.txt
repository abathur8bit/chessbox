cmake_minimum_required(VERSION 3.0.2)
project(chesslrcontroller)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(chesslrcontroller src/main/cpp/controller.cpp src/main/cpp/thc.cpp)
#add_executable(jj src/main/cpp/test.cpp )
target_link_libraries(chesslrcontroller PRIVATE ssobjects wiringPi pthread)
