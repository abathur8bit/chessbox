//compile with: gcc -Wall -o mcptest mcptest.c -lwiringPi
//g++ -o controller -std=c++11 -I../ssobjects -I src/main/cpp -lwiringPi -lssobjects src/main/cpp/controller.cpp
//pin numbers are BCM

/** LEDs are on bank B, piece switches are on bank A.
 *
 * Pin layout:
 *
 * ```
 *                   +----- -----+
 * LED 7 - GPB0 <--> |1    -   28| <--> GPA7 - Reed switch 7
 * LED 6 - GPB1 <--> |2        27| <--> GPA6 - Reed switch 6
 * LED 5 - GPB2 <--> |3        26| <--> GPA5 - Reed switch 5
 * LED 4 - GPB3 <--> |4    M   25| <--> GPA4 - Reed switch 4
 * LED 3 - GPB4 <--> |5    C   24| <--> GPA3 - Reed switch 3
 * LED 2 - GPB5 <--> |6    P   23| <--> GPA2 - Reed switch 2
 * LED 1 - GPB6 <--> |7    2   22| <--> GPA1 - Reed switch 1
 * LED 0 - GPB7 <--> |8    3   21| <--> GPA0 - Reed switch 0
 *          VDD ---> |9    0   20| ---> INTA
 *          VSS ---> |10   1   19| ---> INTB
 *           NC ---- |11   7   18| ---> RESET
 *          SCL ---> |12       17| <--- A2
 *          SDA <--> |13       16| <--- A1
 *           NC ---- |14       15| <--- A0
 *                   +-----------+
 * ```
 *
 * $ sudo i2cdetect -y 1
 *
 *       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
 *  00:          -- -- -- -- -- -- -- -- -- -- -- -- --
 *  10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  20: 20 21 22 23 24 25 26 27 -- -- -- -- -- -- -- --
 *deci> 32 33 34 35 36 37 38 39 <deci
 *  30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 *  70: -- -- -- -- -- -- -- --
 */

#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <mcp23017.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <error.h>
#include <ssobjects/ssobjects.h>
#include <ssobjects/simpleserver.h>
#include <ssobjects/tsleep.h>
#include <ssobjects/telnetserver.h>
#include <ssobjects/telnetserversocket.h>

#include "json.hpp"
#include "thc.h"
#include "chessmove.hpp"
#include "chessaction.hpp"

#define TITLE "Chessbox Controller"
#define VERSION "0.2.0"

#define WAIT_SLOW 500
#define WAIT_FAST 100
#define BASE_I2C 200
#define BASE_I2C_INPUT  BASE_I2C     //first 8 pins (bank A)
#define BASE_I2C_OUTPUT BASE_I2C+8   //last 8 pins (bank B)

using namespace std;
using namespace nlohmann;   //trying this

#define MOVE_UP 'U'
#define MOVE_DOWN 'D'
#define MOVE_NONE '_'

#define LED_OFF 0
#define LED_ON 1
#define LED_FLASH 3

#define SAN_BUF_SIZE 6      ///< Minimum buffer size to hold a san or long san move. Something like long "h7h8q" or san "h8=Q+"

#include "BoardRules.h"

class ControllerServer : public TelnetServer {
public:

    enum {
        MODE_SETUP,
        MODE_INSPECT,       //showing what pieces are detected on the board
        MODE_PLAY,          //normal mode, waiting for the player to make a move
        MODE_MOVE,          //board wants player to move certain pieces; a computer or remote player move.
        MODE_SETPOSITION,   //Board wants player to place pieces in a certain setup.
        MODE_MATE};         //check mate detected, no other moves can be made

    BoardRules rules;
    int gameMode;
    int flashState=0;
    ChessMove waitMove;         ///< The move the board is waiting for the player to complete.
    char moveType[4]= {'_','_','_','_'};
    int moveSquareIndex[4]={-1,-1,-1,-1};
    int moveIndex=0;            ///< Zero indicates not pointing at anything.
    int movesNeeded=0;
    enum {FREQ=10};
    int devId;
    int index;
    int baseInput;
    int baseOutput;
    int input[64];              ///< Pin map for input. You use in your digitalRead calls like **digitalRead(input[squareIndex])**.
    int output[64];             ///< Pin map for output. You use in your digitalWrite calls digitalWrite(output[squareIndex],state);. 1=on, 0=off.
    int squareState[64];        ///< What the board is currently seeing. When a piece is lifted or dropped, this gets updated. 0=empty, 1=occupied
    int ledState[64];           ///< What the LEDs are displaying. If you change this, it will immediately change what is displayed.
    const char* rowNames="87654321";
    const char* colNames="abcdefgh";
    int mcp[8];

    /** Sets up hardware and socket binding. */
    ControllerServer(SockAddr& saBind,bool swap) : TelnetServer(saBind,FREQ){
        devId=0x20;
        index=0;
        baseInput=250-64*2;
        baseOutput=baseInput+8;

        wiringPiSetup();

        for(int row = 0; row<8; ++row) {
            mcp[row]=mcp23017Setup(baseInput,devId+row);
            if(mcp[row]<0)
                perror("wiringPiI2CSetup");

            for(int col=0; col<8; col++) {
                int inputCol = col;
                int outputCol = 7-col;
                pinMode(baseInput+inputCol,INPUT);
                pullUpDnControl(baseInput+inputCol,PUD_UP);
                pinMode(baseOutput+outputCol,OUTPUT);

                input[index]=baseInput+inputCol;
                output[index]=baseOutput+outputCol;

                ++index;
            }
            baseInput+=16;
            baseOutput+=16;
        }

        if(swap) {
            // I messed up my wiring, so this fixes it
            int i=8*4+3;
            int t=input[i];
            input[i]=input[i+1];
            input[i+1]=t;
        }
    }

    void initGame() {
        memset(squareState,0,sizeof(squareState));
        gameMode = MODE_PLAY;
        printf("Turning off led's...\n");
        clearLeds();
        for(int i=0; i<64; i++) {
            squareState[i] = readState(i); //0=empty 1=occupied
        }
//        const char* fen = "8/8/8/8/8/K6k/8/8 w - - 0 1";    //two kings
//        const char* fen = "8/8/8/8/4q3/1K2k3/8/8 w - - 0 1";  //a few pieces for testing
        const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        setPosition(fen);   //lights leds to make sure player places all pieces
        display_position(rules);
        if(!isBoardSetup()) {
            printf("Setup your board as shown\n");
        }
    }

    void processSingleMsg(PacketMessage* pmsg)
    {
        TelnetServerSocket* psocket = (TelnetServerSocket*)pmsg->socket();
        PacketBuffer* ppacket = pmsg->packet();
        switch(ppacket->getCmd())
        {
            //One way to handle the message. Process and reply within the switch.
            case PacketBuffer::pcNewConnection:   onConnection(pmsg);               break;
            case PacketBuffer::pcClosed:          printf("Connection closed.\n");   break;
            case TelnetServerSocket::pcFullLine:  onFullLine(pmsg);                 break;
        }
        DELETE_NULL(ppacket);   //IMPORTANT! The packet is no longer needed. You must delete it.
    }

    ChessAction* parseJson(const char* s) {
        json j = json::parse(s);
        printf("parseJson creating chess action\n");
        ChessAction* c = new ChessAction(j);
        return c;
    }

    void onFullLine(PacketMessage* pmsg)
    {
        TelnetServerSocket* psocket = (TelnetServerSocket*)pmsg->socket();
        char* pszString = (char*)pmsg->packet()->getBuffer();
        printf("Got from client: [%s]\n",pszString);

        try {
            json j = json::parse(pszString);
            json jresult;
            jresult["success"]=false;
            jresult["code"]=nullptr;
            jresult["message"]=nullptr;
            if(j.contains("action")) {
                string action = j["action"];
                printf("parsed and have action = %s\n", action.c_str());
                if (!action.compare("move")) {
                    doMove(psocket, pszString);
                    psocket->println(jresult.dump().c_str());
                } else if (!action.compare("ping")) {
                    psocket->println("pong");
                    jresult["success"]=true;
                    psocket->println(jresult.dump().c_str());
                } else if (!action.compare("setmode")) {
                    setMode(j, jresult);
                    psocket->println(jresult.dump().c_str());
                } else if (!action.compare("led")) {
                    setLed(j, jresult);
                    psocket->println(jresult.dump().c_str());
                } else if (!action.compare("setposition")) {
                    setPosition(j, jresult);
                    psocket->println(jresult.dump().c_str());
                } else if(!action.compare("fen")) {
                    requestFen(psocket);
                } else if(!action.compare("flip")) {

                }
            }

        } catch(json::parse_error& e) {
            psocket->println("json parse error: %s",e.what());
        }
    }

    void requestFen(TelnetServerSocket* sock) {
        json j;
        j["action"]="setposition";
        j["fen"]=rules.ForsythPublish();
        sock->print("%s\n",j.dump().c_str());
    }

    /** Turn on the specified LED. Example:
     * echo '{"action":"led","square":"a2"}' | nc -C -N localhost 9999 */
    void setLed(json& j,json& jresult) {
        jresult["success"] = true;
        string square = j["square"];
        int index = toIndex(square.c_str());
        printf("square=%s index=%d\n",square.c_str(),index);
        led(index,ledState[index]?LED_OFF:LED_ON); //flip from on to off and off to on
    }

    void setPosition(const char* fen) {
        clearLeds();
        rules.Forsyth(fen);
        for(int i=0; i<64; i++) {
            squareState[i] = (rules.pieceAt(i) == ' ' ? 0 : 1);
        }
        gameMode = isBoardSetup() ? MODE_PLAY:MODE_SETPOSITION;
    }
    void setPosition(json& j,json& jresult) {
        string fen = j["fen"];
        setPosition(fen.c_str());
        display_position(rules);
        jresult["success"] = true;
    }

    void setMode(json& j,json& jresult) {
        jresult["success"] = true;     //assume okay
        if(j.count("mode")==1) {
            string mode = j["mode"];
            if(!mode.compare("play")) {
                gameMode = MODE_PLAY;
                jresult["message"] = "game mode set to MODE_PLAY";
                clearLeds();
            } else if(!mode.compare("setup")) {
                gameMode = MODE_SETUP;
                jresult["message"] = "game mode set to MODE_SETUP";
                clearLeds();
            } else if(!mode.compare("inspect")) {
                gameMode = MODE_INSPECT;
                jresult["message"] = "game mode set to MODE_INSPECT";
                clearLeds();
            } else {
                jresult["message"] = "invalid mode";
                jresult["success"] = false;
            }
        } else {
            jresult["message"] = "mode must be specified";
            jresult["success"] = false;
        }
    }

    void clearLeds() {
        for(int i=0; i<64; i++) {
            led(i,LED_OFF);
        }
    }

    void turnOffLeds() {
        for(int i=0; i<64; i++) {
            digitalWrite(output[i], 0);
        }
    }

    void doMove(TelnetServerSocket* psocket,const char* pszString) {
        ChessAction *ca = parseJson(pszString);
        ChessMove m;
        int index=0;
        printf("doMove from=%s to=%s lan=%s\n",ca->move(index).from(),ca->move(index).to(),ca->move(index).lan());
        if(rules.isMoveValid(ca->move(index).lan())) {
            waitMove.setFrom(ca->move(index).fromIndex());
            waitMove.setTo(ca->move(index).toIndex());
            waitMove.setType(ca->move(index).type());
            ledState[ca->move(index).fromIndex()]=1;
            ledState[ca->move(index).toIndex()]=1;
            gameMode=MODE_MOVE;
            moveIndex=0;
            if(!strcmp(ca->move(index).type(), "capture")) {
                moveType[0]=MOVE_UP;
                moveType[1]=MOVE_UP;
                moveType[2]=MOVE_DOWN;
                moveSquareIndex[0]=ca->move(index).fromIndex();
                moveSquareIndex[1]=ca->move(index).toIndex();
                moveSquareIndex[2]=ca->move(index).toIndex();
                movesNeeded=3;
            } else if(!strcmp(ca->move(index).type(), "takeback_capture")) {
                moveType[0]=MOVE_UP;
                moveType[1]=MOVE_DOWN;
                moveType[2]=MOVE_DOWN;
                moveSquareIndex[0]=ca->move(index).fromIndex();
                moveSquareIndex[1]=ca->move(index).fromIndex();
                moveSquareIndex[2]=ca->move(index).toIndex();
                led(ca->move(index).fromIndex(), LED_FLASH);
                movesNeeded=3;
            } else {
                moveType[0]=MOVE_UP;
                moveType[1]=MOVE_DOWN;
                moveSquareIndex[0]=ca->move(index).fromIndex();
                moveSquareIndex[1]=ca->move(index).toIndex();
                movesNeeded=2;
            }

            for(int i=0; i<4; i++) {
                printf("type[%d]=%c square index=%d\n", i, moveType[i], moveSquareIndex[i]);
            }
        } else {
            json j;
            j["action"] = "invalid_move";
            j["lan"] = ca->move(index).lan();
            printf("%s\n",j.dump().c_str());
            psocket->print("%s\n",j.dump().c_str());
            return;
        }
        delete ca;
    }

    void onConnection(PacketMessage* pmsg)
    {
        TelnetServerSocket* psocket = (TelnetServerSocket*)pmsg->socket();
        psocket->println("Welcome to %s v%s", TITLE,VERSION);
        printf("New connection\n");
    }

    /** Returns a string like "a1". */
    char* toMove(char* buffer, size_t n, char col, char row) {
        snprintf(buffer, n, "%c%c", col, row);
        return buffer;
    }
    char* toMove(char* buffer,size_t n,int index) {
        snprintf(buffer,n,"%c%c",toCol(index),toRow(index));
        return buffer;
    }

    /** Return the letter character of the column the index points to. The column should display before the row. */
    char toCol(int index) {
        int y = index/8;
        int x = index-y*8;
        return colNames[x];
    }

    /** Returns the number character of the row the index points to. You should use the column, then row. */
    char toRow(int index) {
        int y = index/8;
        int x = index-y*8;
        return rowNames[y];
    }

    /** Convert a string like "A1" into an index like 56, or "A8" to 0. Top left corner is 0, bottom right is 63. */
    int toIndex(const char* square) {
        int col=tolower(square[0])-'a';
        int row=8-(square[1]-'0');
//        printf("%s col=%d row=%d\n",square,col,row);
        return row*8+col;
    }

    int toIndex(char col,char row) {
        char buffer[SAN_BUF_SIZE];
        toMove(buffer,sizeof(buffer),col,row);
        return toIndex(buffer);
    }

    //called every FREQ milliseconds
    void idle(unsigned32 now) {
        switch(gameMode) {
            case MODE_INSPECT: idleShowPieces(); break;
            case MODE_PLAY: idlePlay(); break;
            case MODE_MOVE: idleMove(); break;
            case MODE_SETPOSITION: idleSetPosition(); break;
        }
        flasher();
    }

    //Using ledState array, turns LEDs on, and flashes them if the flash bit is set
    void flasher() {
        flashState = !flashState;
        for(int i=0; i<64; i++) {
            int on=ledState[i]&1;
            if(ledState[i]&2)
                on=flashState;
            digitalWrite(output[i],on?1:0);
        }
    }

    void idleShowPieces() {
        char buffer[1024];
        for (int i = 0; i < 64; i++) {
            int state = readState(i);
            if (state != squareState[i]) {
                snprintf(buffer, sizeof(buffer), "%c%c %s\n", toCol(i), toRow(i), (state ? "pieceDown" : "pieceUp"));
                printf(buffer);
                send2All(buffer);
            }
            squareState[i] = state;
            led(i, state?LED_ON:LED_OFF);
        }
    }

    /**
     * Turns on LEDs for any valid destination the piece at fromIndex can make.
     *
     * @param fromIndex Square that has the piece player is moving.
     * @return true if there is at least one valid move, false otherwise.
     */
    bool showValidSquares(int fromIndex) {
        char bufFrom[SAN_BUF_SIZE],bufTo[SAN_BUF_SIZE];
        snprintf(bufFrom, sizeof(bufFrom), "%c%c", toCol(fromIndex), toRow(fromIndex));
//        printf("Move from %s\n",bufFrom);

        std::vector<thc::Move> moves;
        std::vector<bool> check;
        std::vector<bool> mate;
        std::vector<bool> stalemate;
        rules.GenLegalMoveList(moves, check, mate, stalemate);
        unsigned int len = moves.size();
        int validMoves=0;
        clearLeds();
        led(fromIndex,LED_ON);
        for(int i=0; i<len; i++) {
            thc::Move mv = moves[i];
            std::string mv_txt = mv.TerseOut();
//            printf("checking move %s > %s\n",mv_txt.c_str(),bufFrom);
            if(mv_txt[0] == bufFrom[0] && mv_txt[1] == bufFrom[1]) {
                int to=toIndex(mv_txt[2],mv_txt[3]);
                led(to,1);
                validMoves++;
            }
        }
//        printf("number of valid moves %d\n",len);
        return validMoves>0;
    }

    /** To long algebraic notation like "a2a3". */
    char* toLAN(char* dest,size_t n,int from,int to) {
        char bfrom[SAN_BUF_SIZE];
        char bto[SAN_BUF_SIZE];
        char bmove[SAN_BUF_SIZE];
        toMove(bfrom,sizeof(bfrom),from);
        toMove(bto,sizeof(bto),to);
        snprintf(dest,n,"%s%s",bfrom,bto);
        return dest;
    }

    void display_position(thc::ChessRules& cr)
    {
        std::string fen = cr.ForsythPublish();
        std::string s = cr.ToDebugStr();
        printf("FEN = %s\n", fen.c_str());
        printf("Position = %s\n", s.c_str());
    }

    /** Flash the king of the current player. */
    void flashKingCheck() {
        int index=0;
        for(int i=0; i<64; i++) {
            if((rules.pieceAt(i) == 'k' && !rules.WhiteToPlay()) || (rules.pieceAt(i) == 'K' && rules.WhiteToPlay())) {
                index = i;
                break;
            }
        }

        long delay=100000;
        turnOffLeds();  //force the leds off, instead of waiting for next idle call
        usleep(delay);
        digitalWrite(output[index],1);
        usleep(delay);
        digitalWrite(output[index],0);
        usleep(delay);
        digitalWrite(output[index],1);
        usleep(delay);
        digitalWrite(output[index],0);
    }

    /** Look to see if we are in checkmate, and set checkmated king's square to flash. */
    void evaluateCheckMate() {
        thc::TERMINAL terminal;
        rules.Evaluate(terminal);
        switch(terminal) {
            case thc::TERMINAL::TERMINAL_BCHECKMATE: printf("black checkmate\n"); break;
            case thc::TERMINAL::TERMINAL_WCHECKMATE: printf("white checkmate\n"); break;
        }
        if(terminal==thc::TERMINAL::TERMINAL_BCHECKMATE || terminal==thc::TERMINAL::TERMINAL_WCHECKMATE) {
            gameMode=MODE_MATE;
            for(int i=0; i<64; i++) {
                if(rules.pieceAt(i) == 'k' && terminal == thc::TERMINAL::TERMINAL_BCHECKMATE)
                    led(i,LED_FLASH);
                else if(rules.pieceAt(i) == 'K' && terminal == thc::TERMINAL::TERMINAL_WCHECKMATE)
                    led(i,LED_FLASH);
            }
        }
    }

    /** Check if there is a draw. */
    void evaluateDraw() {
        //todo lee implement evaluate draw

        //        thc::DRAWTYPE drawType;
//        rules.IsDraw(rules.WhiteToPlay(),drawType);
//        switch(drawType) {
//            case thc::DRAWTYPE::DRAWTYPE_50MOVE: printf("DRAWTYPE_50MOVE\n"); break;
//            case thc::DRAWTYPE::DRAWTYPE_INSUFFICIENT: printf("DRAWTYPE_INSUFFICIENT\n"); break;
//            case thc::DRAWTYPE::DRAWTYPE_INSUFFICIENT_AUTO: printf("DRAWTYPE_INSUFFICIENT_AUTO\n"); break;
//            case thc::DRAWTYPE::DRAWTYPE_REPITITION: printf("DRAWTYPE_DRAWTYPE_REPITITION"); break;
//        }
//        if(drawType != thc::DRAWTYPE::NOT_DRAW) {
//            for(int i=0; i<64; i++) {
//                if(rules.pieceAt(i) == 'k' || rules.pieceAt(i) == 'K')
//                    led(i,LED_FLASH);
//            }
//        }
    }

    /** Piece was put down, now check what the move was. */
    void finishMove(int toIndex,bool sendMove) {
        //todo didn't flash the rooks square when player castled, but did when computer wanted to move it
        moveIndex = 0;
        clearLeds();
        if(toIndex != moveSquareIndex[0]) {
            //this is a move, player didn't replace the piece on the square they lifted it off from
            char buffer[SAN_BUF_SIZE];
            toLAN(buffer, sizeof(buffer), moveSquareIndex[0], toIndex);
            thc::Move mv;
            mv.TerseIn(&rules, buffer);
            printf("full move is %s - %s\n", buffer,mv.NaturalOut(&rules).c_str());
            if(!mv.Valid()) {
                json j;
                j["action"] = "invalid_move";
                j["lan"] = buffer;
                printf("%s\n",j.dump().c_str());
                send2All(j.dump().c_str());
                send2All("\n");
                setPosition(rules.ForsythPublish().c_str());
                return;
            }
            string san = mv.NaturalOut(&rules).c_str();
            bool kingChecked = rules.isCheck(mv);
            bool capture = mv.NaturalOut(&rules).find('x')!=string::npos;

            vector<json> moveList;
            json move;
            move["type"]=capture ? "capture":"move";
            move["from"]=toMove(buffer,sizeof(buffer),moveSquareIndex[0]);
            move["to"]=toMove(buffer,sizeof(buffer),toIndex);
            move["lan"] = mv.TerseOut().c_str();
            move["san"] = san;
            moveList.push_back(move);

            if(sendMove) {
                json j;
                j["action"] = "move";
                j["description"] = nullptr;
                j["moves"]=moveList;
                printf("%s\n",j.dump().c_str());

                send2All(j.dump().c_str());
                send2All("\n");
            }
            rules.PlayMove(mv);
//            display_position(rules);
//            printf("san=%s check=%d kingChecked=%d\n",san.c_str(),san.find_first_of('+'),kingChecked);
            if(kingChecked) {
                flashKingCheck();
            }
        }
        setPosition(rules.ForsythPublish().c_str());
        evaluateCheckMate();
        evaluateDraw();
    }

    void idlePlay() {
        //check each square to see if its state has changed
        for (int i = 0; i < 64; i++) {
            int state = readState(i);
            if (state != squareState[i]) {
                //state 0=piece lifted, 1=piece dropped
                squareState[i] = state;
                char buffer[SAN_BUF_SIZE];
                toMove(buffer, sizeof(buffer), toCol(i), toRow(i));
                json j;
                j["action"] = state ? "pieceDown" : "pieceUp";
                j["square"] = buffer;
                printf("%s state=%d moveIndex=%d\n",j.dump().c_str(),state,moveIndex);
                send2All(j.dump().c_str());
                send2All("\n");

                if (!state && !moveIndex) {
                    //picked up first piece
                    printf("picked up first piece\n");
                    if(showValidSquares(i)) {
                        moveType[moveIndex] = MOVE_UP;
                        moveSquareIndex[moveIndex] = i;
                        moveIndex++;
                    } else {
                        //todo lee would like to be able to pick up the piece you are capturing first
                        printf("You can't move that piece\n");
                        while(!readState(i)) {
                            usleep(100000);
                            digitalWrite(output[i],1);
                            usleep(100000);
                            digitalWrite(output[i],0);
                            usleep(100000);
                            digitalWrite(output[i],1);
                            usleep(100000);
                            digitalWrite(output[i],0);
                            usleep(300000);
                        }
                        led(i,LED_OFF);
                        squareState[i] = readState(i);
                        j["action"] = squareState[i] ? "pieceDown" : "pieceUp";
                        j["square"] = buffer;
                        printf("%s state=%d moveIndex=%d\n",j.dump().c_str(),state,moveIndex);
                        send2All(j.dump().c_str());
                        send2All("\n");
                    }
                } else if(!state && moveIndex<2) {
                    //picked up another piece
                    printf("picked up a second piece\n");
                    moveType[moveIndex] = MOVE_UP;
                    moveSquareIndex[moveIndex] = i;
                    moveIndex++;
                    led(i, LED_ON);
                } else if(!state && moveIndex>=2) {
                    //picked up more then 2 pieces
                    printf("picked up too many pieces\n");
                    setPosition(rules.ForsythPublish().c_str());
                } else if(state && !moveIndex) {
                    //piece down but no piece up, not valid
                    printf("put down a piece but none picked up\n");
                    setPosition(rules.ForsythPublish().c_str());
                } else if(state) {
                    //piece down
                    printf("put down piece\n");
                    moveType[moveIndex] = MOVE_DOWN;
                    moveSquareIndex[moveIndex] = i;
                    moveIndex++;
                    finishMove(i,true);
                } else {
                    printf("WARNING should not get here\n");
                    assert(false);
                }
                break;  //only process one square change at a time
            }
        }
    }

    /** We are in move mode, waiting for player to finish making a designated move. */
    void idleMove() {
        for (int i = 0; i < 64 && gameMode==MODE_MOVE; i++) {
            int state = readState(i);
            if (state != squareState[i]) {
                char type = state ? MOVE_DOWN:MOVE_UP;
                if(moveType[moveIndex] == type && moveSquareIndex[moveIndex] == i) {
                    printf("got move %d %c\n",moveIndex,moveType[moveIndex]);
                    bool resetLed = (moveIndex==1 && type==MOVE_UP) ? false:true;
                    if(resetLed)
                        led(i,LED_OFF);
                    moveIndex++;
                    if(moveIndex == movesNeeded) {
                        //got all the moves we were waiting for
                        gameMode = MODE_PLAY;
                        printf("Move finished json=\n",waitMove.tojson().dump().c_str());
//                        send2All(waitMove.tojson().dump().c_str());
//                        send2All("\n");
                        finishMove(moveSquareIndex[moveIndex-1],false);
                    }
                } else {
                    char buffer[SAN_BUF_SIZE];
                    snprintf(buffer, sizeof(buffer), "%c%c", toCol(i), toRow(i));
                    json j;
                    j["action"] = state ? "pieceDown" : "pieceUp";
                    j["square"] = buffer;
                    printf("%s\n",j.dump().c_str());
                    send2All(j.dump().c_str());
                    send2All("\n");
                }
            }
            squareState[i] = state;
        }
    }

    /** Any squares that need a piece, will be solid. Any square that has a piece that should not
     * will flash.
     */
    void idleSetPosition() {
        //setup what it should look like
        bool complete=true;
        for (int i = 0; i < 64; i++) {
            int state = readState(i);
            if(squareState[i] != state) {
                complete=false;
                if(state && !squareState[i])
                    led(i,LED_FLASH);   //flash
                else if(!state && squareState[i])
                    led(i,LED_ON);   //just turn it on
            } else {
                led(i,LED_OFF);
            }
        }
        if(complete) {
            clearLeds();
            gameMode = MODE_PLAY;
//            json j;
//            j["action"] = "setposition";
//            j["status"] = "complete";
//            printf("%s\n",j.dump().c_str());
//            send2All(j.dump().c_str());
//            send2All("\n");
        }
    }

    /** Checks if all squares have a piece that should, and the ones that should not are empty. */
    bool isBoardSetup() {
        bool setup=true;
        for (int i = 0; i < 64; i++) {
            int state = readState(i);
            if(state != squareState[i])
                setup=false;
        }
        return setup;
    }

    /**
     * Checks if a piece is detected on the given square.
     * @param index Square to check.
     * @return 0 if empty, 1 if a piece is detected.
     */
    int readState(int index) {
        return digitalRead(input[index])==0 ? 1:0;
    }

    // Return true if the square is occupied, false otherwise.
    bool isSquareOccupied(int index) {
        return squareState[index]==1;   //0=occupied, 1=not
    }

    // Return true if the state given means the square is occupied, false otherwise
    bool isStateOccupied(int state) {
        return state==0;
    }

    /** Sets the LED state, doesn't actually turn on/off the led. */
    void led(int index,int state) {
        ledState[index] = state;
    }
};

void usage(const char* name) {
    printf("%s {-h,--help,-s,-p,-l}\n",name);
    printf("  --help,-h This message\n");
    printf("  -s Swap switches for Lee's board\n");
    printf("  -p What port to run on\n");
    printf("  -l,--leds-off Turn off all LEDs and exit\n");
    exit(0);
}

int main(int argc,char* argv[]) {
    bool swap=false;
    bool turnOffLeds=false;
    unsigned16 wPort = 9999;
    for(int i=0; i<argc; i++) {
        if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help"))
            usage(argv[0]);
        else if(!strcmp(argv[i],"-s")) {
            swap=true;
        } else if(!strcmp(argv[i],"-p")) {
            wPort = atoi(argv[++i]);
        } else if(!strcmp(argv[i],"-l") || !strcmp(argv[i],"--leds-off")) {
            turnOffLeds=true;
        }
    }
    printf("Binding to port %d\n",wPort);
    SockAddr saBind((ULONG)INADDR_ANY,wPort);
    printf("%s runs on port %d\n",TITLE,wPort);
    ControllerServer server(saBind,swap);
    if(turnOffLeds) {
        printf("Turning off leds\n");
        server.turnOffLeds();
    } else {
        server.initGame();
        server.startServer();
    }
    printf("%s finished\n",TITLE);

//    auto output = make_shared<string>();
//    auto error = make_shared<string>();
//
//Process p("/home/pi/workspace/chessengine/stockfish-8-linux/src/stockfish","",[output](const char *bytes, size_t n) {
//    *output = string(bytes, n);
//    printf("--> %s\n",output->c_str());
//}, nullptr,true);
//p.write("uci\n");
//p.write("quit\n");
//printf("process exit=%d output=[%s]\n",p.get_exit_status(),output->c_str());

    return 0;
}