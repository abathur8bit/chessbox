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
#include "process.hpp"
#include <mutex>
#include <list>

using namespace TinyProcessLib;
using namespace std;

list<string> queue;
std::mutex mtx;

void push(string s) {
    std::unique_lock<std::mutex> lck(mtx);
    queue.push_back(s);
}

string pull() {
    std::unique_lock<std::mutex> lck(mtx);
    if(queue.empty())
        return "";
    string s=queue.front();
    queue.pop_front();
    return s;
}

//http://cplusplus.com/reference/mutex/mutex/
//to have parent thread wait for child thread to notify it: http://cplusplus.com/reference/condition_variable/condition_variable/
void startProcess(const char* filepath) {
    auto output = make_shared<string>();
    auto error = make_shared<string>();

    Process p(filepath,"",[output](const char *bytes, size_t n) {
        *output = string(bytes, n);
        push(*output);
        printf("--> %s\n",output->c_str());
        if(output->find("Hello") != string::npos)
            printf("HELLO found!\n");
    }, nullptr,true);
    p.write("uci\n");
    time_t timer=time(NULL)+2;
    while(time(NULL)<timer) {
        printf("tick..");
    }
    printf("\n");
    p.write("quit\n");
    int status=0;
    while(!p.try_get_exit_status(status)) {
        printf("process exit=%d\n",status);
        string s=pull();
        if(!s.empty()) {
            printf("got output [%s]\n",s.c_str());
        }
    }
    printf("Listing output:\n");
    string s=pull();
    while(!s.empty()) {
        printf("[%s]\n",s.c_str());
        s=pull();
    }
}

void snooze() {
    time_t timer=time(NULL)+3;
    while(time(NULL)<timer) {}
}
void foo() {
    char buffer[80];
    for(int i=0; i<10; i++) {
        snprintf(buffer,sizeof buffer,"foo %d",i);
        push(buffer);
    }
}
int main(int argc,char* argv[]) {
    if(argc<=1) {
        char buffer[80]="";
        printf("Hello process, type 'quit' to stop echoing\n");
        while(strcmp(buffer, "quit")) {
            fgets(buffer, sizeof buffer, stdin);
            buffer[strlen(buffer) - 1]='\0';  //remove newline char that fgets leaves
            printf("Hello %s!\n", buffer);
        }
    } else {
//        startProcess(argv[1]);
//        thread first(foo);
        thread first ([](){char buffer[80];
            for(int i=0; i<10; i++) {
                snprintf(buffer,sizeof buffer,"foo %d",i);
                push(buffer);
            }});
        printf("adding data\n");
        push("one");
        push("two");
        push("three");
        push("four");
        push("five");
        first.join();

        printf("Listing output:\n");
        string s=pull();
        while(!s.empty()) {
            printf("[%s]\n",s.c_str());
            s=pull();
        }
    }
}