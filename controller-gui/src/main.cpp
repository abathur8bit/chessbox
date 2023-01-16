/* *****************************************************************************
 * Created by Lee Patterson 12/19/2020
 *
 * Copyright 2019 Lee Patterson <https://github.com/abathur8bit>
 *
 * You may use and modify at will. Please credit me in the source.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ******************************************************************************/
//-e "C:\Program Files\tarrasch-v3.12b-g\Engines\stockfish_8_x64.exe" -h 192.168.1.54
//-e "C:/Program Files (x86)/ShredderChess/Deep Shredder 13/EngineDeepShredder13UCIx64.exe" -h 192.168.1.54
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <list>
#include <stdlib.h>

#include <SDL.h>
#include <time.h>

#include "Sprite.h"
#include "Button.h"
#include "Board.h"
#include "thc.h"
#include "Label.h"
#include "MovesPanel.h"
#include "Connector.h"
#include "json.hpp"
#include "ControllerGUI.h"
#include "prefs.h"

using namespace std;
using namespace nlohmann;   //trying this

#include <algorithm>
#include <cctype>
#include <locale>

int main(int argc, char* argv[]) {
    printf("You may need to run 'export SDL_VIDEODRIVER=rpi'\n");
    Prefs prefs("cbgui.json");
    int numdrivers, i, working;
    const char* drivername;

    if (SDL_Init(0) != 0) {
        printf("Error initializing SDL:  %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    numdrivers = SDL_GetNumVideoDrivers();
    working = 0;

    for (i = 0; i < numdrivers; ++i) {
        drivername = SDL_GetVideoDriver(i);

        if (SDL_VideoInit(drivername) == 0) {
            SDL_VideoQuit();
            ++working;
            printf("Driver %s works.\n", drivername);
        }
        else {
            printf("Driver %s doesn't work.\n", drivername);
        }
    }

    printf("\n%d video drivers total (%d work)\n", numdrivers, working);

    prefs.read();
    printf("prefs host=%s, port=%d, fullscreen=%d\n",prefs.host(),prefs.port(),prefs.isFullscreen());
    ControllerGUI gui(prefs.isFullscreen(),prefs.host(),prefs.port(),prefs.engine(),prefs.currentGame());
    gui.startGame();
    return 0;
}