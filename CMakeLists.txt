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

add_executable(controller_gui controller-gui/src/main.cpp controller-gui/src/Sprite.cpp controller-gui/src/Sprite.h controller-gui/src/Button.cpp controller-gui/src/Button.h controller-gui/src/Board.cpp controller-gui/src/Board.h controller-gui/src/Component.cpp controller-gui/src/Component.h controller-gui/src/UIGroup.cpp controller-gui/src/UIGroup.h)
target_link_libraries(controller_gui PRIVATE SDL2main SDL2 SDL2_image SDL2_ttf)
