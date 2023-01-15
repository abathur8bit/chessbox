/* *****************************************************************************
 * Created by Lee Patterson 12/31/2020
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

#ifndef CONTROLLER_GUI_PGNUTILS_H
#define CONTROLLER_GUI_PGNUTILS_H

#include <string>

#include "BoardRules.h"

using namespace std;

#define PGN_EMPTY "?"
#define PGN_RESULT_WHITE_WIN "1-0"
#define PGN_RESULT_BLACK_WIN "1-0"
#define PGN_RESULT_NO_WIN "*"

/** Utils for reading and writing a PGN file. */
class PGNUtils {
public:
    PGNUtils(){}
    bool save(const char* pathname,BoardRules* rules,string result=PGN_RESULT_NO_WIN,string white=PGN_EMPTY,string black=PGN_EMPTY,long round=-1,string date=PGN_EMPTY,string event=PGN_EMPTY,string site=PGN_EMPTY,string fen=PGN_EMPTY);
    bool save(ostream& out,BoardRules* rules,string result=PGN_RESULT_NO_WIN,string white=PGN_EMPTY,string black=PGN_EMPTY,long round=-1,string date=PGN_EMPTY,string event=PGN_EMPTY,string site=PGN_EMPTY,string fen=PGN_EMPTY);

protected:
    char m_buffer[1024];
};


#endif //CONTROLLER_GUI_PGNUTILS_H
