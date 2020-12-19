cmake_minimum_required(VERSION 3.13.4)
project(controller_gui)

set(CMAKE_CXX_STANDARD 14)

if(WIN32)
    include_directories(/workspace/sdl/SDL2/include)
    link_directories(/workspace/sdl/SDL2/lib/x86)
else()
    include_directories(/usr/local/include/SDL2)
    link_directories(/usr/local/lib)
endif()

add_executable(controller_gui src/main.cpp src/Sprite.cpp src/Sprite.h src/Button.cpp src/Button.h src/Board.cpp src/Board.h src/Component.cpp src/Component.h src/UIGroup.cpp src/UIGroup.h)
target_link_libraries(controller_gui PRIVATE SDL2main SDL2 SDL2_image SDL2_ttf)
