cmake_minimum_required(VERSION 3.0.2)
project(cbcontroller)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(../common)
include_directories(../tiny-process-library)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
link_directories(../tiny-process-library/build)

add_executable(cbcontroller src/main/cpp/controller.cpp ../common/thc.cpp)
add_executable(process src/main/cpp/process.cpp)
#add_executable(jj src/main/cpp/test.cpp )
target_link_libraries(cbcontroller PRIVATE ssobjects wiringPi pthread tiny-process-library)
