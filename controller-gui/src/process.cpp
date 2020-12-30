/* *****************************************************************************
 * Created by Lee Patterson 12/29/2020
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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <time.h>
#include <mutex>
#include <list>

#include "UCIClient.h"

void usage() {
    printf("process <engine_path>\n");
    printf("engine_path is a path to the UCI engine\n");
    exit(0);
}
//./process
int main(int argc,char* argv[]) {
    if(argc<2) usage();

    string engine="/home/pi/workspace/chessengine/stockfish-8-linux/src/stockfish";
    string fen="8/p4kpp/2r5/2P1b3/8/7P/P5P1/4R2K w - -";
    for(int i=0; i<argc; i++) {
        if(!strcmp(argv[i],"-e")) {
            engine=argv[++i];
        } else if(!strcmp(argv[i],"-f")) {
            fen=argv[++i];
        }
    }

    UCIClient uci(engine);
    uci.setDebug(false);
    uci.start();
    uci.sendCommand("uci");
    uci.newGame();
    string move;
    string ponder;
    uci.bestMove(move,ponder,fen);
    printf("best move was [%s] ponder was [%s]\n",move.c_str(),ponder.c_str());

//
//    string s=uciPull();
//    while(!s.empty()) {
//        printf("read line [%s]\n",s.c_str());
//        s=uciPull();
//    }
//
//    printf("best move was [%s]\n",move.c_str());
//
//    uci.bestMove(&move,&ponder,fen);
//    printf("best move was [%s] ponder was [%s]\n",move.c_str(),ponder.c_str());

    uci.stop();
}