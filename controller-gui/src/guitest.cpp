/* *****************************************************************************
 * Created by Lee Patterson 1/2/2021
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

#include "UCIClient.h"
#include <map>
#include <typeinfo>
#include <iostream>
using namespace std;

int main(int argc,char* argv[]) {
    printf("Hello World\n");
    UCIClient uci("C:\\workspace\\chessbox\\stockfish_20090216_x64_avx2.exe");
    uci.setDebug(false);
    uci.start();
    uci.discoverOptions();
    EngineOption* op=uci.option("name");
    cout << "name: " << op->currentValue() << endl;
    op=uci.option("Skill Level");
    cout << "option " << op->name() << " value "<<op->currentValue()<<endl;
//    list<string> options;
//    string line="";
//    do {
//        uciWait();
//        line=uciPull();
//        if(line.find("option") != string::npos) {
//            options.push_back(line);
//        }
//        printf("Option=[%s]\n",line.c_str());
//    } while(line.compare("uciok"));
//
//    list<EngineOption*> engineOptions;
//    for(list<string>::iterator it=options.begin(); it!=options.end(); it++) {
//        cout << "Option ["<<*it<<"]"<<endl;
//        EngineOption* op=uci.parseOption(*it);
//        if(op!=nullptr) {
//            engineOptions.push_back(op);
//        cout << "Option ["<<*it<<"]"<<endl;
//        } else {
//            cout << "Ignoring option ["<<*it<<"]"<<endl;
//
//        }
//    }
}


