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
#include <SDL_image.h>

#include "Sprite.h"
#include "Button.h"
#include "Board.h"
#include "UIGroup.h"
#include "thc.h"
#include "Label.h"
#include "MovesPanel.h"
#include "Connector.h"
#include "chessaction.hpp"
#include "json.hpp"
#include "ControllerGUI.h"
#include "Dialog.h"
#include "TextField.h"
#include "FontManager.h"

using namespace std;
using namespace nlohmann;   //trying this

char buffer[1024];
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 800;
int counter=0;
//#define NUM_MOVES 16

SDL_Window* window=NULL;
SDL_Renderer* renderer=NULL;
bool invalidated = true;
//list<Component*> uistuff;
//UIGroup buttonGroup("buttons",0,670,280,130);
//UIGroup movesGroup("moves",480-200,480,200,800-320);

//Connector connector;
bool attemptConnect=true;
bool running=false;
//Board board(0,0,480,480);
//MovesPanel* movesPanel;
//Sprite logo("logo");
//Sprite movesBG("moves-bg1");
//Label* blackClockText;
//Label* whiteClockText;
//int gameMovesIndex=0;
//const char* gameMoves[] = {
//        "d4","d5",
//        "c4","c6",
//        "Nf3","Nf6",
//        "Nc3","e6",
//        "Bg5","h6",
//        "Bxf6","Qxf6",
//        "e3","Nd7",
//        "Rc1","g6",
//        "Be2","Bg7",
//        "cxd5","exd5",
//        "b4","a6",
//        "a4","O-O",
//        "b5","axb5",
//        "axb5","Qd6",
//        "O-O","Nb6",
//        "Qb3","Rb8",
//        "Nd1","Bf5",
//        "Nb2","Rfc8",
//        "Nd3","Bxd3",
//        "Qxd3","c5",
//        "dxc5","Rxc5",
//        "h4","Na4",
//        "h5","Rbc8",
//        "Rxc5","Nxc5",
//        "Qc2","gxh5",
//        "Nd4","Qg6",
//        "Nf5","Bf8",
//        "Rd1","Qe6",
//        "Rc1","Nb3",
//        "Qxc8","Nxc1",
//        "Qxc1","Qxf5",
//        "Qc7","Qb1+",
//        "Bf1","d4",
//        "exd4","Qd1",
//        "Qe5","Bg7",
//        "Qe8+","Bf8",
//        "Qd8","Kg7",
//        "Qd5","b6",
//        "Qe5+","Kg8",
//        "Qf6","Bg7",
//        "Qxb6","Bxd4",
//        "Qxh6","Qg4",
//        "Qd6","Qd1",
//        "Qd8+","Kh7",
//        "Qc7","Kg7",
//        "b6","Qg4",
//        "b7","Qh4",
//        "g3","Qf6",
//        "Qc2","Qe5",
//        "Qd3","Ba7",
//        "Qf3","Qf6",
//        "Qe2","Qc3",
//        "Kh2","Qd4",
//        "Qf3","Bb8",
//        "Kh3","Bc7",
//        "Be2","Bb8",
//        "Bd1","f5",
//        "Be2","f4",
//        "Qxh5","Qxf2",
//        "Qg5+","Kf7"
//};


//void validate() {
//    invalidated=false;
//}
//
//void invalidate() {
//    invalidated=true;
//}
//
//bool isInvalidated() {
//    return invalidated;
//}
//
//
//
//

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