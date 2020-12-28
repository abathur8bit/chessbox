/* *****************************************************************************
 * Created by Lee Patterson 12/28/2020
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
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Engine process
typedef struct {
    pid_t pid;
    FILE *in, *out, *log;
    int inhandle;
    char name[64];
} Engine;

bool engine_start(Engine *e, const char *cmd, FILE *log)
{
    // Pipe diagram: Parent -> [1]into[0] -> Child -> [1]outof[0] -> Parent
    // 'into' and 'outof' are pipes, each with 2 ends: read=0, write=1
    int outof[2], into[2];

    if (pipe(outof) < 0 || pipe(into) < 0)
        return false;

    e->pid = fork();

    if (e->pid == 0) {
        // in the child process
        close(into[1]);
        close(outof[0]);

        if (dup2(into[0], STDIN_FILENO) == -1)
            return false;
        close(into[0]);

        if (dup2(outof[1], STDOUT_FILENO) == -1)
            return false;
        close(outof[1]);

        if (execlp(cmd, cmd, NULL) == -1)
            return false;
    } else if (e->pid > 0) {
        // in the parent process
        close(into[0]);
        close(outof[1]);

        if (!(e->in = fdopen(outof[0], "r")))
            return false;

        if (!(e->out = fdopen(into[1], "w")))
            return false;

        // FIXME: doesn't work on Windows
        setvbuf(e->in, NULL, _IONBF, 0);
        setvbuf(e->out, NULL, _IONBF, 0);
    } else {
        // fork failed
        return false;
    }
    e->log = log;
    return true;
}

bool engine_stop(Engine *e)
{
    // close the parent side of the pipes
    fclose(e->in);
    fclose(e->out);

    // terminate the child process
    return kill(e->pid, SIGTERM) >= 0;
}

bool engine_readln(const Engine *e, char *buf, size_t n)
{
    printf("reading\n");
//    fd_set rset;
//    FD_ZERO(&rset);
//    FD_SET(e->in,&rset);
//    struct timeval tv={0,0};
//    int ready=select(e->in+1,&rset,NULL,NULL,&tv);
//    printf("ready=%d\n",ready);
//    if(-1==ready) {
//        printf("select error\n");
//    } else if(ready>0) {
        if(fgets(buf, n, e->in)) {
            printf("done reading\n");
            if(e->log)
                fprintf(e->log, "%s -> %s", e->name, buf);

            return true;
        }
        printf("error reading\n");

        return false;
//    } else {
//        no data available
//        buf[0]='\0';
//        return true;
//    }
//    return false;
}

bool engine_writeln(const Engine *e, const char *buf)
{
    printf("writing \n");
    if (fputs(buf, e->out) >= 0) {
        printf("done write\n");
        if (e->log)
            fprintf(e->log, "%s <- %s", e->name, buf);

        return true;
    }
    printf("error writing\n");

    return false;
}

int main(int argc,char* argv[]) {
    printf("Hello world\n");
    Engine engine;
    FILE* logfile = fopen("log.txt","w");
    if(!logfile) {
        printf("Unable to open log file\n");
        exit(1);
    }

    if(engine_start(&engine,"/home/pi/workspace/chessengine/stockfish-8-linux/src/stockfish",logfile)) {
        printf("Engine pid=%d\n", engine.pid);
        char line[1024];
        engine_readln(&engine, line, sizeof line);
        printf("Read: %s\n", line);
        engine_writeln(&engine, "uci\n");
        do {
            engine_readln(&engine, line, sizeof line);
            printf("Read: %s\n", line);
        } while(strlen(line)>0);
        return 0;
    } else {
        printf("unable to launch engine\n");
        return 1;
    }
}