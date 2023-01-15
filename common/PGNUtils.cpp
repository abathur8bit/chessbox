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

#include <iostream>     // std::cout, std::ios
#include <sstream>
#include <fstream>

#include <time.h>
#include "PGNUtils.h"

/**
 * Save a PGN notated file.
 *
 * @param pathname  File path where to save the game to.
 * @param rules     Board rules for the game to be saved.
 * @param result    One of "1-0,0-1,1/2-1/2,*"
 * @param white     Name of the white player.
 * @param black     Name of the black player.
 * @param round     Round or -1.
 * @param date      Date in YYYY.MM.DD format.
 * @param event     Name of the event or "?".
 * @param site      Site of the game or "?".
 * @param fen       Position of the game, like "7k/5P2/8/5K2/8/7p/8/8 w - - 0 1"
 * @return          true if saved okay, false otherwise.
 */
bool PGNUtils::save(const char* pathname,BoardRules* rules,string result,string white,string black,long round,string date,string event,string site,string fen) {
    filebuf fb;
    fb.open(pathname,std::ios::out);
    std::ostream os(&fb);
    save(os,rules,result,white,black,round,date,event,site,fen);
    fb.close();

    return true;
}

/** Save a PGN notated file to the output stream. Stream can be a file stream, or string stream. */
bool PGNUtils::save(ostream& out,BoardRules* rules,string result,string white,string black,long round,string date,string event,string site,string fen) {
    string sround="?";
    if(-1!=round) {
        snprintf(m_buffer,sizeof(m_buffer),"%d",round);
        sround=m_buffer;
    }
    if(date.compare(PGN_EMPTY)==0) {
        time_t rawtime;
        struct tm* timeinfo;
        time(&rawtime);
        timeinfo=localtime(&rawtime);
        strftime(m_buffer,sizeof m_buffer,"%Y.%m.%d",timeinfo);
        date=m_buffer;
    }

    out << "[Event \""<< event << "\"]" << endl;
    out << "[Site \""<< site << "\"]" << endl;
    out << "[Date \""<< date << "\"]" << endl;
    out << "[Round \""<< sround << "\"]" << endl;
    out << "[White \""<< white << "\"]" << endl;
    out << "[Black \""<< black << "\"]" << endl;
    out << "[Result \""<< result << "\"]" << endl;
    if(fen.compare(PGN_EMPTY)!=0) {
        out << "[SetUp \"1\"]" << endl;
        out << "[FEN \"" << fen << "\"]" << endl;
    }

    if(rules->historyIndex()>1) {
        int i=1;
        int moveNum=1;
        BoardRules pgnRules;
        while(i<rules->historyIndex()) {
            if(i+1<rules->historyIndex()) {
                //white
                out << endl << moveNum++ << ". " << rules->historySanAt(i) << " ";
                //black
                out << rules->historySanAt(++i);
            } else {
                //we have just a white move
                out << endl << moveNum++ << ". " << rules->historySanAt(i) << " ";
            }
            ++i;
        }
    }

    out << " " << result;
    return true;
}
