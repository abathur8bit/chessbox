/* *****************************************************************************
 * Created by Lee Patterson 1/3/2021
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

/*************
 * Uses CxxTest 4.4. Should be run on a machine with Python.
 *
 * pi@millipede:~/workspace/chessbox$ cat tests.sh
 * #!/bin/bash
 * # $@ - all command line params
 * # $1 - first param
 * # $# - number of command line params
 * set -e
 * ~/workspace/cxxtest-4.4/bin/cxxtestgen --error-printer -o runner.cpp common/ucitest.hpp
 * g++ -g -I/home/pi/workspace/cxxtest-4.4 -Icommon -Icontroller/src -Icontroller-gui/src -Itiny-process-library -o runner runner.cpp common/UCIClient.cpp -Ltiny-process-library/build  -ltiny-process-library -lpthread
 * ./runner -v
 *
 * Once tests.sh has been run, runner should be built, and you can run with runner -h or running --help-tests
 * and you can run an individual test with runner ClassName MethodName.
 */
#include <cxxtest/TestSuite.h>
#include "UCIClient.h"
#include "BoardRules.h"

class TestPGN : public CxxTest::TestSuite
{
public:
    void testMove() {
        BoardRules rules;
        rules.playMove("a4");
        rules.display_position();
        cout << "terse  : " << rules.historyAt(0).TerseOut() << endl;
        cout << "natural: " << rules.historyAt(0).NaturalOut(&rules) << endl;

    }
};

class TestUCIParsing : public CxxTest::TestSuite
{
public:
    void testInvalidTypeIsNull() {
        UCIClient uci;
        EngineOption* op=uci.parseOption("option name UCI_EngineAbout type XXX default Shredder by Stefan Meyer-Kahlen, www.shredderchess.com");
        if(op!=NULL) {
            TS_TRACE("op is suppose to be null");
        }
        TS_ASSERT(op==NULL);

    }
    void testStringOption(void) {
        UCIClient uci;
        EngineOption* op=uci.parseOption("option name UCI_EngineAbout type string default Shredder by Stefan Meyer-Kahlen, www.shredderchess.com");
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(op->name(),"UCI_EngineAbout");
        TS_ASSERT_EQUALS(op->type(),option_string);
        TS_ASSERT_EQUALS(op->defaultValue(),"Shredder by Stefan Meyer-Kahlen, www.shredderchess.com");

        op=uci.parseOption("option name UCI_EngineAbout type string default");
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(op->name(),"UCI_EngineAbout");
        TS_ASSERT_EQUALS(op->type(),option_string);
        TS_ASSERT_EQUALS(op->defaultValue(),"");
    }
    //spin option with default first and max last (Stockfish)
    void testSpinOption() {
        UCIClient uci;
        EngineOption *op=uci.parseOption("option name Minimum Thinking Time type spin default 20 min 0 max 5000");
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(op->name(), "Minimum Thinking Time");
        TS_ASSERT_EQUALS(op->type(), option_spin);
        EngineSpinOption *spin=static_cast<EngineSpinOption *>(op);
        TS_ASSERT_EQUALS(spin->defaultValue(), "20");
        TS_ASSERT_EQUALS(spin->minValue(), 0);
        TS_ASSERT_EQUALS(spin->maxValue(), 5000);
    }

    //make sure we handle having default at the end (Shredder)
    void testSpinDefaultLast() {
        UCIClient uci;
        EngineOption* op=uci.parseOption("option name Contempt type spin min -100 max 100 default 20");
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(op->name(),"Contempt");
        TS_ASSERT_EQUALS(op->type(),option_spin);
        EngineSpinOption* spin=static_cast<EngineSpinOption*>(op);
        TS_ASSERT_EQUALS(spin->defaultValue(),"20");
        TS_ASSERT_EQUALS(spin->minValue(),-100);
        TS_ASSERT_EQUALS(spin->maxValue(),100);
    }

    void testCheckOption() {
        UCIClient uci;
        EngineOption* op=uci.parseOption("option name Ponder type check default false");
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(op->name(),"Ponder");
        TS_ASSERT_EQUALS(op->type(),option_check);
        EngineCheckOption* check=static_cast<EngineCheckOption*>(op);
        TS_ASSERT_EQUALS(check->isChecked(),false);

        op=uci.parseOption("option name Ponder Something type check default true");
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(op->name(),"Ponder Something");
        TS_ASSERT_EQUALS(op->type(),option_check);
        check=static_cast<EngineCheckOption*>(op);
        TS_ASSERT_EQUALS(check->isChecked(),true);

        //a value other then true or false should equate to true
        check=static_cast<EngineCheckOption*>(uci.parseOption("option name Ponder Something type check default XXX"));
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(check->isChecked(),true);
    }

    void testComboOption() {
        UCIClient uci;
        EngineOption* op=uci.parseOption("option name Analysis Contempt type combo default Both var Off var White var Black var Both");
        TS_ASSERT(op!=nullptr);
        TS_ASSERT_EQUALS(op->name(),"Analysis Contempt");
        TS_ASSERT_EQUALS(op->type(),option_combo);
        EngineComboOption* combo=static_cast<EngineComboOption*>(op);
        list<string>::iterator it=combo->begin();
        TS_ASSERT_EQUALS(*it++,"Off");
        TS_ASSERT_EQUALS(*it++,"White");
        TS_ASSERT_EQUALS(*it++,"Black");
        TS_ASSERT_EQUALS(*it++,"Both");
    }
};

//class TestUCIOptionsWithEngine : public CxxTest::TestSuite {
//public:
//    void testThing() {
//        UCIClient uci("./engine/stockfish-8-linux/src/stockfish");
//        uci.setDebug(true);
//        uci.start();
//        uciWait();
//        uciPull();  //ignoring first line
//
//        uci.sendCommand("uci");
//        list<string> options;
//        string line;
//        while(line.compare("uciok")) {
//            line=uci.waitFor("option");
//        }
//        for(list<string>::iterator it=options.begin(); it!=options.end(); it++) {
//            TS_TRACE(*it);
//        }
//
//        uci.stop();
//    }
//};