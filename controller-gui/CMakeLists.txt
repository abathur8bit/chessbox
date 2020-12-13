cmake_minimum_required(VERSION 3.13.4)
project(controller_gui)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../sdl/SDL2/include)
link_directories(C:/workspace/sdl/SDL2/lib/x86)

#if(MSVC)
##    C:\workspace\chessbox\controller-gui\CMakeLists.txt
#    include_directories(../../sdl/SDL2/include)
#    link_directories(/../../sdl/SDL2/lib/x64)
#else()
#    include_directories(/usr/local/include/SDL2)
#    link_directories(/usr/local/lib)
#endif()

add_executable(controller_gui main.cpp Game.cpp logs.cpp TextureManager.cpp)
target_link_libraries(controller_gui PRIVATE SDL2main SDL2 SDL2_image)
