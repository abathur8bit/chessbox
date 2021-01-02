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

#include <time.h>
#include "PGNUtils.h"

bool PGNUtils::save(const char* pathname,BoardRules* rules,string result,string white,string black,long round,string date,string event,string site,string fen) {
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

    FILE* fp;
    fp=fopen(pathname,"w");
    if(!fp)
        return false;

    fprintf(fp,"[Event \"%s\"]\n",event.c_str());
    fprintf(fp,"[Site \"%s\"]\n",site.c_str());
    fprintf(fp,"[Date \"%s\"]\n",date.c_str());
    fprintf(fp,"[Round \"%s\"]\n",sround.c_str());
    fprintf(fp,"[White \"%s\"]\n",white.c_str());
    fprintf(fp,"[Black \"%s\"]\n",black.c_str());
    fprintf(fp,"[Result \"%s\"]\n",result.c_str());
    if(fen.compare(PGN_EMPTY)!=0) {
        fprintf(fp,"[SetUp \"1\"]\n");
        fprintf(fp,"[FEN \"%s\"]\n",fen.c_str());
    }
    fprintf(fp,"\n");

    if(rules->historyIndex()>1) {
        int i=1;
        int moveNum=1;
        while(i<rules->historyIndex()) {
            if(i+1<rules->historyIndex()) {
                //we have a white and black move
                Move mvwhite = rules->historyAt(i);
                bool whiteCheck=rules->isCheck(mvwhite);
                Move mvblack = rules->historyAt(++i);
                bool blackCheck=rules->isCheck(mvblack);
                fprintf(fp,"%d. %s %s",moveNum++,mvwhite.TerseOut().c_str(),mvblack.TerseOut().c_str());
                if(i+1==rules->historyIndex() && rules->isMate())
                    fprintf(fp,"# ");
                else
                    fprintf(fp,"\n");
            } else {
                //we have just a white move
                Move mv = rules->historyAt(i);
                bool whiteCheck=rules->isCheck(mv);
                mv.NaturalIn(rules,"e8=Q+");
                string san=mv.NaturalOut(rules);
                printf("san=%s\n",san.c_str());
                fprintf(fp,"%d. %s",moveNum++,mv.TerseOut().c_str());
                if(i+1==rules->historyIndex() && rules->isMate())
                    fprintf(fp,"# ");
                else
                    fprintf(fp," ");
            }
            ++i;
        }
    }

    fprintf(fp,"%s\n\n",result.c_str());

    fclose(fp);
    return true;
}
