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

using namespace std;
using namespace nlohmann;   //trying this

int main(int argc, char* argv[]) {
    printf("You may need to run 'export SDL_VIDEODRIVER=rpi'\n");
    char host[80]="chessbox";
    unsigned short port=9999;
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

    bool fullscreen=false;
#ifdef WIN32
    string pgn="/t/game.pgn";
    string engine="C:/Program Files (x86)/ShredderChess/Deep Shredder 13/EngineDeepShredder13UCIx64.exe";
#else
    string pgn="/home/pi/game.pgn";
    string engine="engine/stockfish8";
#endif
    for(int i=1; i<argc; i++) {
        if(!strcmp("-f",argv[i])) {
            fullscreen=true;
        } else if(!strcmp("-h",argv[i]) && i+1 <= argc) {
            strncpy(host,argv[++i],sizeof(host));
            NULL_TERMINATE(host, sizeof(host));
        } else if(!strcmp(argv[i],"-e")) {
            engine=argv[++i];
        } else if(!strcmp("-p",argv[i])) {
            pgn=argv[++i];
        }
    }
    ControllerGUI gui(fullscreen,host,port,engine.c_str(),pgn.c_str());
    gui.startGame();
    return 0;
}