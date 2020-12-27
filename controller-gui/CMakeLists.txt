cmake_minimum_required(VERSION 3.13.4)
project(controller_gui)

set(CMAKE_CXX_STANDARD 14)

include_directories(../common)

if(WIN32)
    include_directories(/workspace/sdl/SDL2/include)
    link_directories(/workspace/sdl/SDL2/lib/x86)
else()
    include_directories(/usr/local/include/SDL2)
    link_directories(/usr/local/lib)
endif()

if(WIN32)
    add_executable(controller_gui ../common/thc.cpp ../common/thc.h src/main.cpp src/Sprite.cpp src/Sprite.h src/Button.cpp src/Button.h src/Board.cpp src/Board.h src/Component.cpp src/Component.h src/UIGroup.cpp src/UIGroup.h src/Label.cpp src/Label.h src/MovesPanel.cpp src/MovesPanel.h src/Connector.cpp src/Connector.h ../common/generalexception.cpp ../common/generalexception.h ../common/socketinstance.cpp ../common/socketinstance.h src/ControllerGUI.cpp src/ControllerGUI.h src/ControllerGUIProto.cpp src/ControllerGUIProto.h src/Dialog.cpp src/Dialog.h src/TextField.cpp src/TextField.h src/FontManager.cpp src/FontManager.h src/Window.cpp src/Window.h)
else()
    add_executable(cbgui          ../common/thc.cpp ../common/thc.h src/main.cpp src/Sprite.cpp src/Sprite.h src/Button.cpp src/Button.h src/Board.cpp src/Board.h src/Component.cpp src/Component.h src/UIGroup.cpp src/UIGroup.h src/Label.cpp src/Label.h src/MovesPanel.cpp src/MovesPanel.h src/Connector.cpp src/Connector.h                                                                                                                     src/ControllerGUI.cpp src/ControllerGUI.h src/ControllerGUIProto.cpp src/ControllerGUIProto.h src/Dialog.cpp src/Dialog.h src/TextField.cpp src/TextField.h src/FontManager.cpp src/FontManager.h src/Window.cpp src/Window.h)
endif()

if(WIN32)
    target_link_libraries(controller_gui PRIVATE SDL2main SDL2 SDL2_image SDL2_ttf Ws2_32.lib)
else()
    target_link_libraries(cbgui PRIVATE SDL2main SDL2 SDL2_image SDL2_ttf ssobjects)
endif()
