#include <SDL.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cstdio>
#include <cctype>
#include <vector>
#include <sstream>
#include <string>
#include <utility>
#include "telnetserver.h"
#include "json.hpp"
#include "Window.h"
#include "FontManager.h"
#include "Label.h"
#include "version-sim.h"

#ifdef WIN32
WSADATA m_wsd;              ///< WSA startup information (Windows only)
#endif

using namespace nlohmann;   //json
using namespace std;

#define FLASH_SPEED 100
#define BUTTON_WIDTH    60
#define BUTTON_HEIGHT   40
#define SCREEN_WIDTH    400
#define SCREEN_HEIGHT   400
#define NUM_SQUARES     64

#define GAME_MODE_IDLE  0
#define GAME_MODE_SETUP 1
#define GAME_MODE_MOVE  2
#define GAME_MODE_END   3
#define GAME_MODE_DEMO  4
#define GAME_MODE_PLAYING  5
#define MOVE_UP 'U'
#define MOVE_DOWN 'D'
#define PIECE_DOWN "piece_down"
#define PIECE_UP "piece_up"
#define SETUP_BUTTON_ID "setupbuttonid"
#define CLEAR_BUTTON_ID "clearbuttonid"
static const auto APP_STARTED = std::chrono::system_clock::now();
static const auto APP_NAME = "Chessbox Simulator";

static string format_utc_time(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_utc{};

#ifdef _WIN32
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif

    std::ostringstream out;
    out << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}

static string format_uptime(std::chrono::seconds total) {
    auto secs = total.count();

    long long days = secs / 86400;
    secs %= 86400;
    long long hours = secs / 3600;
    secs %= 3600;
    long long minutes = secs / 60;
    secs %= 60;

    std::ostringstream out;
    bool wrote = false;

    if (days > 0) {
        out << days << "d ";
        wrote = true;
    }
    if (wrote || hours > 0) {
        out << hours << "h ";
        wrote = true;
    }
    if (wrote || minutes > 0) {
        out << minutes << "m ";
    }
    out << secs << "s";

    return out.str();
}

struct Request {
    Request(std::string id, std::string action)
            : id(std::move(id)), action(std::move(action)) {}
    Request(json j) {
        id = j["id"];
        action = j["action"];
    }
    virtual ~Request() = default;

    json to_json() const {
        json j;
        j["id"] = id;
        j["action"] = action;
        json k = params_to_json();
        if(!k.is_null()) j["params"] = k;
        return j;
    }

    virtual json params_to_json() const {return nullptr;}

    std::string id;
    std::string action;
    json params;
};

struct HelloAction : Request {
    HelloAction(std::string version, std::string message)
            : Request("HELO", "HELO"),
              version(std::move(version)),
              message(std::move(message)) {}
    HelloAction(json j) : Request(j) {
        message = j["params"]["message"];
        version = j["params"]["version"];
    }
    json params_to_json() const override {
        return json{
                {"version", version},
                {"message", message}
        };
    }
    std::string version;
    std::string message;
};

struct ChessError {
    string code;
    string field;
    string message;
    json to_json() {
        return json{
                {"code",code},
                {"field",field},
                {"message",message},
        };
    }
};

struct Response {
    string id;
    bool success;

    Response(string id,bool success) : id(move(id)),success(success) {}
    virtual ~Response() = default;

    json to_json() const {
        json j;
        j["id"] = id;
        j["success"] = success;
        json k = result_to_json();
        if(!k.is_null()) j["params"] = k;
        k = errors_to_json();
        if(!k.is_null()) j["errors"] = k;
        return j;
    }

    virtual json result_to_json() const {return nullptr;}
    virtual json errors_to_json() const {return nullptr;}
};


inline void to_json(json& j, const ChessError& e) {
    j = json{
            {"code", e.code},
            {"field", e.field},
            {"message", e.message}
    };
}
struct ErrorResponse : Response {
    vector<ChessError> errors;

    ErrorResponse(string id) : Response(move(id), false) {}

    void addError(ChessError error) {
        errors.push_back(move(error));
    }

    json errors_to_json() const override {
        if (errors.empty()) return nullptr;
        return errors;
    }
};


struct SetLedRequest : Request {
    SetLedRequest(std::string id, bool on, std::vector<std::string> squares)
            : Request(std::move(id), "led-set"),
              on(on),
              squares(std::move(squares)) {}

    SetLedRequest(const json& j)
            : Request(j) {
        const auto& p = j.at("params");
        p.at("on").get_to(on);
        p.at("squares").get_to(squares);
    }

    json params_to_json() const override {
        return json{
                {"on", on},
                {"squares", squares}
        };
    }

    bool on;
    vector<string> squares;
};
struct QuerySquareRequest : public Request {
    QuerySquareRequest(std::string id, std::vector<std::string> squares)
    : Request(std::move(id), "query-squares"),
    squares(std::move(squares)) {}

    QuerySquareRequest(const json& j)
            : Request(j) {
        const auto& p = j.at("params");
        p.at("squares").get_to(squares);
    }

    json params_to_json() const override {
        return json{
                {"squares", squares}
        };
    }

    vector<string> squares;
};
struct LedState {
    string square;
    string state;   //off,on,flashing
};
struct PieceState {
    string square;
    bool pieceOn;
};


inline void to_json(json& j, const LedState& e) {
    j = json{
            {"square", e.square},
            {"state", e.state}
    };
}inline void to_json(json& j, const PieceState& e) {
    j = json{
            {"square", e.square},
            {"piece-on", e.pieceOn}
    };
}
struct QuerySquareResponse : public Response {
    QuerySquareResponse(string id) : Response(id,true) {}
    vector<PieceState> pieces;
    vector<LedState> leds;
    json result_to_json() const override {
        return json{
            {"piece-state", pieces},
            {"led-state", leds}
        };
    }
};

struct Pong : Response {
    string pong;
    string utc;
    string started;
    string uptime;
    long long seconds;
    string version;
    string name;

    Pong(
            string id,
            string name,
            string version,
            std::chrono::system_clock::time_point app_started
    )
            : Response(move(id), true),
              pong("Ping? Pong!"),
              seconds(0),
              version(move(version)),
              name(move(name)) {
        auto now = std::chrono::system_clock::now();
        auto up = std::chrono::duration_cast<std::chrono::seconds>(now - app_started);

        utc = format_utc_time(now);
        started = format_utc_time(app_started);
        uptime = format_uptime(up);
        seconds = up.count();
    }

    json result_to_json() const override {
        return json{
                {"pong", pong},
                {"utc", utc},
                {"started", started},
                {"uptime", uptime},
                {"seconds", seconds},
                {"version", version},
                {"name", name}
        };
    }
};





class SimBoard : public Component {
protected:
    SDL_Rect m_rectSquare;
    SDL_Color m_blackColor;
    SDL_Color m_whiteColor;
    bool m_flashState = false;
    bool m_led[NUM_SQUARES];
    bool m_pieces[NUM_SQUARES];
    bool m_flash[NUM_SQUARES];
    const char m_lan[NUM_SQUARES][3] = {
            "a8","b8","c8","d8","e8","f8","g8","h8",
            "a7","b7","c7","d7","e7","f7","g7","h7",
            "a6","b6","c6","d6","e6","f6","g6","h6",
            "a5","b5","c5","d5","e5","f5","g5","h5",
            "a4","b4","c4","d4","e4","f4","g4","h4",
            "a3","b3","c3","d3","e3","f3","g3","h3",
            "a2","b2","c2","d2","e2","f2","g2","h2",
            "a1","b1","c1","d1","e1","f1","g1","h1"
    };

public:
    SimBoard(int x, int y, int w, int h) : Component("board", x, y, w, h) {
        m_rectSquare = {x, y, w / 8, h / 8};
        m_whiteColor = {0x4B,0x99,0xC5,0xFF};
        m_blackColor = {0x00,0x6A,0xA6,0xFF};

        for(int i=0; i<NUM_SQUARES; i++) {
            m_led[i]=false;
            m_pieces[i]=false;
            m_flash[i]=false;
        }
    }

    void toggleFlash() {
        m_flashState = !m_flashState;
    }

    /// Returns the board index or -1 if lan is not between a1 and h8
    int fromLan(const char* lan) {
        if (lan == nullptr) return -1;
        if (lan[0] < 'a' || lan[0] > 'h') return -1;
        if (lan[1] < '1' || lan[1] > '8') return -1;
        if (lan[2] != '\0') return -1;

        int file = lan[0] - 'a';   // a=0, b=1, ... h=7
        int rank = lan[1] - '0';   // '1'..'8' -> 1..8

        return (8 - rank) * 8 + file;
    }

    const char* lan(int pos) {
        return m_lan[pos];
    }
    const char* lan(int x, int y) {
        return lan(y * 8 + x);
    }
    void setPiece(int n,bool state) {
        m_pieces[n] = state;
    }
    void setPiece(int x, int y, bool state) {
        setPiece(y * 8 + x,state);
    }
    bool piece(int x, int y) {
        return piece(y * 8 + x);
    }
    bool piece(int n) {
        return m_pieces[n];
    }
    void setLed(int x, int y, bool state) {
        setLed(y*8+x,state);
    }
    void setLed(int n,bool state) {
        m_led[n] = state;
    }
    void setLedAll(bool state) {
        for(int i=0; i<NUM_SQUARES; i++) {
            m_led[i] = state;
        }
    }
    bool led(int n) {
        return m_led[n];
    }
    bool led(int x, int y) {
        return led(y * 8 + x);
    }
    void setFlashing(int n, bool state) {
        m_flash[n] = state;
    }
    bool isFlashing(int n) {
        return m_flash[n];
    }
    bool isFlashing(int x, int y) {
        return isFlashing(y * 8 + x);
    }

    void queryPieces(bool *dest) {
        for(int i=0; i<NUM_SQUARES; i++) {
            dest[i] = m_pieces[i];
        }
    }
    void queryLeds(bool *dest) {
        for(int i=0; i<NUM_SQUARES; i++) {
            dest[i] = m_led[i];
        }
    }

    void draw(SDL_Renderer *renderer) {
        drawSquares(renderer);
    }
    void drawSquares(SDL_Renderer *renderer) {
        int white=1;
        int i=0;
        SDL_Color color;
        for(int y=0; y<8; y++) {
            for(int x=0; x<8; x++) {
                int xx= m_rectSquare.x + m_rectSquare.w * x;
                int yy= m_rectSquare.y + m_rectSquare.h * y;
                SDL_Rect r = {xx, yy, m_rectSquare.w, m_rectSquare.h};
                if(white) {
                    color = m_whiteColor;
                    white=0;
                } else {
                    color = m_blackColor;
                    white=1;
                }
                if(m_pieces[i]) {
                    color.r+=100;
                    color.g+=100;
                    color.b+=100;
                }
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderFillRect(renderer, &r);

                SDL_Color ledColor = {255,255,255,255};
                if(m_flash[i] && !m_flashState) {
                    ledColor = {0,0,0,255};
                }
                if(m_led[i] || m_flash[i]) {
                    SDL_SetRenderDrawColor(renderer, ledColor.r, ledColor.g, ledColor.b, ledColor.a);
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                }
                SDL_Rect led = {xx+5, yy+5, 5, 5};
                SDL_RenderFillRect(renderer, &led);
                i++;
            }
            white=!white;
        }
    }
};

class SimServer : public TelnetServer, Window {
protected:
    char m_buffer[1024];
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SimBoard m_board;
    int m_expectedPosition[64];
    long m_lastTicks;
    int m_gameMode = GAME_MODE_IDLE;
    int m_moveActions[4];
    int m_moveLocations[4];
    int m_movesNeeded=0;
    int m_moveIndex=0;
    Label* m_pstatusLabel;
public:
    SimServer(const SockAddr& saBind) :
            TelnetServer(saBind),
            Window("chessbox sim",0,0,SCREEN_WIDTH,SCREEN_HEIGHT+60),
            m_window(nullptr),
            m_renderer(nullptr),
            m_board(0, 0, SCREEN_WIDTH, SCREEN_WIDTH),
            m_lastTicks(0),
            m_pstatusLabel(nullptr)
    {
        memset(m_expectedPosition,0,sizeof m_expectedPosition);
        memset(m_moveActions,0,sizeof m_moveActions);
        memset(m_moveLocations,0,sizeof m_moveLocations);
        m_movesNeeded = 0;
        m_moveIndex = 0;

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
            return;
        }
        m_window = SDL_CreateWindow(
                "Chessbox Simulator",
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                50,50,
                SCREEN_WIDTH, SCREEN_HEIGHT+BUTTON_HEIGHT,
                SDL_WINDOW_RESIZABLE);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        setupFonts();
        m_renderer = SDL_CreateRenderer(m_window, -1, 0);
        if(!m_renderer) {
            printf("m_renderer error %s\n",SDL_GetError());
        }
        addLogo();
        addStatusLabel(saBind);

        setupDefaultChessPosition();
        draw(m_renderer);
        SDL_RenderPresent(m_renderer);
    }
    ~SimServer() {
        SDL_DestroyWindow(m_window);
        SDL_DestroyRenderer(m_renderer);
        if(m_pstatusLabel) {
            delete m_pstatusLabel;
            m_pstatusLabel = nullptr;
        }
        SDL_Quit();
    }
    void setupFonts() {
        FontManager::instance()->add("small","Inconsolata-Medium.ttf",10);
        FontManager::instance()->add("normal","Inconsolata-Medium.ttf",16);
        FontManager::instance()->add("large","Inconsolata-Medium.ttf",26);
    }
    void addLogo() {
        Sprite* logo=new Sprite("logo");
        logo->load(m_renderer, "assets/logo-sm.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        addComponent(logo);
    }
    void addStatusLabel(const SockAddr& saBind) {
        //status label to show what port sim is waiting on
        char buff[100];
        snprintf(buff,sizeof(buff),"Sim waiting for connection on port %u",saBind.port());
        m_pstatusLabel = new Label("status", 10, 10, SCREEN_WIDTH, SCREEN_WIDTH);
        m_pstatusLabel->setText(buff);
        addComponent(m_pstatusLabel);
    }
    virtual void connected(SocketInstance sock,SockAddr& sa) {
        char buff[1024];
        snprintf(buff,sizeof(buff),"Connection from %s opened\n",sa.dottedDecimal());
        m_pstatusLabel->setText(buff);
        auto action = HelloAction(APP_VERSION,"Chessboard says hello");
        println(sock, action.to_json().dump().c_str());

        addComponent(&m_board);
        int x=0,y=SCREEN_HEIGHT,w=BUTTON_WIDTH,h=BUTTON_HEIGHT,gap=5;
        TextButton* level=new TextButton(SETUP_BUTTON_ID,"Setup",x,y,w,h);
        addButton(level);
        addComponent(level);
        TextButton* clearbn=new TextButton(CLEAR_BUTTON_ID,"Clear",x+w+1,y,w,h);
        addButton(clearbn);
        addComponent(clearbn);

        draw(m_renderer);
        SDL_RenderPresent(m_renderer);
    }
    virtual void closed(SocketInstance sock,SockAddr& sa) {
        printf("Connection from %s closed\n",sa.dottedDecimal());
    }
    int parseFrom(const char* lan) {
        int col=tolower(lan[0])-'a';
        int row=8-(lan[1]-'1')-1;
        return row*8+col;
    }
    int parseTo(const char* lan) {
        int col=tolower(lan[2])-'a';
        int row=8-(lan[3]-'1')-1;
        return row*8+col;
    }
    int toPos(int x,int y) {
        return y*8+x;
    }
    virtual void processLine(SocketInstance sock,SockAddr& sa,char* line) {
        printf("Got [%s] from client\n",line);
        try {
            json src=json::parse(line);
            json result;
            result["success"]=false;
            if(src.contains("action")) {
                string action=src["action"];
                if(action.compare("quit")==0) {
                    //if we quit, the socket gets closed
                    quit(sock, src, result);
                } else {
                    if(action.compare("ping")==0) ping(sock, Request(src), result);
//                    else if(action.compare("setup-pieces")==0) setupPieces(sock, src, result);
                    else if(action.compare("flash")==0) flash(sock, SetLedRequest(src), result);
                    else if(action.compare("set-led")==0) led(sock, SetLedRequest(src), result);
//                    else if(action.compare("move")==0) move(sock, src, result);
                    else if(action.compare("query-squares")==0) querySquares(sock, QuerySquareRequest(src), result);
                    println(sock, result.dump().c_str());
                }
            } else {
                ErrorResponse resp = ErrorResponse("0");
                resp.addError({"INVALID_JSON", "action", "Field is required"});
                println(sock,resp.to_json().dump().c_str());
            }
        } catch (json::exception& ex) {
            printf("parse error at byte %s\n",ex.what());
            fflush(stdout);
            ErrorResponse resp = ErrorResponse("ERROR");
            resp.addError({"INVALID_JSON", "INVALID_JSON", "Invalid request message"});
            println(sock,resp.to_json().dump().c_str());
        }
    }

    void quit(SocketInstance sock,json& src,json& result) {
        result["success"]=true;
        result["message"]="Good bye";
        println(sock,result.dump().c_str());
        sock.close();
    }
    void ping(SocketInstance sock,const Request& ping,json& result) {
        auto pong = Pong(ping.id,APP_NAME,APP_VERSION,APP_STARTED);
        result = pong.to_json();
    }
    void flash(SocketInstance sock, const SetLedRequest& req, json& result) {
        for(auto sq : req.squares) {
            m_board.setFlashing(parseFrom(sq.c_str()), req.on);
        }
        result = Response(req.id,true).to_json();
    }
    void led(SocketInstance sock, const SetLedRequest& req, json& result) {
        for(auto sq : req.squares) {
            m_board.setLed(parseFrom(sq.c_str()), req.on);
        }
        result = Response(req.id,true).to_json();
    }
    void move(SocketInstance sock,json& src,json& result) {
        string lan=src["lan"];
        int from=parseFrom(lan.c_str());
        int to=parseTo(lan.c_str());
        if(m_board.piece(from)==true && m_board.piece(to)==false) {
            printf("move from=%d,to=%d\n", from, to);
            m_gameMode=GAME_MODE_MOVE;
            m_moveActions[0]=MOVE_UP;
            m_moveActions[1]=MOVE_DOWN;
            m_moveIndex=0;
            m_movesNeeded=2;
            m_moveLocations[0]=from;
            m_moveLocations[1]=to;
            m_board.setLed(from, true);
            m_board.setLed(to, true);
            result["success"]=true;
        }
    }
    void querySquares(SocketInstance sock,const QuerySquareRequest& req,json& result) {
        QuerySquareResponse resp = QuerySquareResponse(req.id);
        for(auto sq : req.squares) {
            int index = parseFrom(sq.c_str());
            resp.pieces.push_back({sq,m_board.piece(index)});
            string state="off";
            if(m_board.isFlashing(index))
                state="flashing";
            else if(m_board.led(index))
                state = "on";
            resp.leds.push_back({sq,state});
        }
        result = resp.to_json();
    }
    void queryLeds(SocketInstance sock,json& src,json& result) {
        json leds = json::array({});
        for(int i=0; i<NUM_SQUARES; i++) {
            if(m_board.led(i)) {
                leds.push_back(i);
            }
        }
        result["leds_on"] = leds;
        result["success"] = true;
    }
    void queryPieces(SocketInstance sock,json& src,json& result) {
        json switches = json::array({});
        for(int i=0; i<NUM_SQUARES; i++) {
            if(m_board.piece(i)) {
                switches.push_back(i);
            }
        }
        result["has_piece"] = switches;
        result["success"] = true;
    }
    void setupPieces(SocketInstance sock,json& src,json& result) {
            vector<int> squares = src["squares"];
            m_board.setLedAll(false);
            m_gameMode = GAME_MODE_IDLE;                //assume the board is setup correctly
            memset(m_expectedPosition,0,sizeof m_expectedPosition);
            for(int i=0; i<NUM_SQUARES; i++) {
                //check if this square should be set
                if(containsSquare(i,squares)) {
                    m_expectedPosition[i] = 1;
                    if(!m_board.piece(i)) {
                        m_board.setLed(i,true);
                        m_gameMode = GAME_MODE_SETUP;   //user has to manipulate the board, go into setup mode.
                    }
                } else {
                    if(m_board.piece(i)) {
                        m_board.setFlashing(i,true);
                        m_gameMode = GAME_MODE_SETUP;   //user has to manipulate the board, go into setup mode.
                    }
                }
            }
            result["success"]=true;
    }
    void processPieceUp(int pos) {
        m_board.setPiece(pos,false);

        if(m_board.isFlashing(pos)) {
            m_board.setFlashing(pos,false);
        } else if(m_gameMode == GAME_MODE_MOVE && pos != m_moveLocations[m_moveIndex]) {
            m_board.setFlashing(pos, true);
        } else {
            if(m_sClient!=0) {
                json j;
                j["success"]=true;
                j["action"]=PIECE_UP;
                j["square"]=pos;
                j["lan"]=m_board.lan(pos);
                println(m_sClient, j.dump().c_str());
            }

            if(m_gameMode==GAME_MODE_MOVE) {
                if(m_moveActions[m_moveIndex]==MOVE_UP && pos==m_moveLocations[m_moveIndex]) {
                    printf("got move up %d %c\n", m_moveIndex, m_moveActions[m_moveIndex]);
                    m_board.setLed(pos, false);
                    m_moveIndex++;
                }
            }
        }
    }
    void processPieceDown(int pos) {
        m_board.setPiece(pos,true);

        if(m_gameMode==GAME_MODE_SETUP) {

        }
        if(m_board.isFlashing(pos)) {
            m_board.setFlashing(pos, false);
        } else {
            if(m_sClient!=0) {
                json j;
                j["success"]=true;
                j["action"]=PIECE_DOWN;
                j["square"]=pos;
                j["lan"]=m_board.lan(pos);
                println(m_sClient, j.dump().c_str());
            }

            if(m_gameMode==GAME_MODE_MOVE) {
                if(m_moveActions[m_moveIndex]==MOVE_DOWN && pos==m_moveLocations[m_moveIndex]) {
                    printf("got move down %d %c\n", m_moveIndex, m_moveActions[m_moveIndex]);
                    m_board.setLed(pos, false);
                    m_moveIndex++;
                    if(m_moveIndex==m_movesNeeded) {
                        m_gameMode=GAME_MODE_PLAYING;
                        printf("game mode playing\n");
                        json j;
                        j["success"]=true;
                        j["action"]="move";
                        j["from"]=m_moveLocations[0];
                        j["to"]=m_moveLocations[m_moveIndex - 1];
                        println(m_sClient, j.dump().c_str());
                    }
                } else {
                    m_board.setFlashing(pos, true);
                }
            }
        }
    }

    //turn off all pieces and leds
    void clearAllPiecesLeds() {
        for(int i=0; i<64; i++) {
            m_board.setPiece(i, false);
            m_board.setLed(i, false);
            m_board.setFlashing(i,false);
        }
    }

    //turn on pieces where new game would be
    void setupDefaultChessPosition() {
        //put pieces on A8 thru H7
        for(int i=0; i<16; i++) {
            m_board.setPiece(i, true);
        }
        //put pieces on A2 thru H1
        for(int i=48; i<64; i++) {
            m_board.setPiece(i, true);
        }
    }

    virtual void processButtonClicked(Button* b) {
        printf("process mouse\n");
        if(!strcmp(b->id(), SETUP_BUTTON_ID)) {
            clearAllPiecesLeds();
            setupDefaultChessPosition();
            m_gameMode = GAME_MODE_PLAYING;
        } else if(!strcmp(b->id(), CLEAR_BUTTON_ID)) {
            clearAllPiecesLeds();
        }
    }

    void mouse(SDL_Event* event) {
        printf("button x=%d y=%d\n",event->button.x,event->button.y);
        int w=SCREEN_WIDTH / 8;
        int h=SCREEN_WIDTH / 8;
        int x=event->button.x / w;
        int y=event->button.y / h;
        if(event->button.y<SCREEN_WIDTH) {    //yup width, since the board is square
            int pos=toPos(x, y);
            if(m_board.piece(pos)) {
                processPieceUp(pos);
            } else {
                processPieceDown(pos);
            }
        }
    }

    virtual void idle() {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    mouse(&event);
                    break;
                case SDL_MOUSEBUTTONUP:
                    processMouseEvent(&event);
                    break;
                case SDL_QUIT:
                    m_showing=false;
                    break;
                case SDL_KEYDOWN:
                    if(SDL_SCANCODE_ESCAPE==event.key.keysym.scancode)
                        m_showing=false;
                    break;
                default:
                    break;
            }
        }
        draw(m_renderer);
        SDL_RenderPresent(m_renderer);
        update(SDL_GetTicks());
        switch(m_gameMode) {
            case GAME_MODE_MOVE: idleMove(); break;
        }
    }
    virtual void update(long ticks) {
        Window::update(ticks);
        long delta = ticks-m_lastTicks;
        if(delta>=FLASH_SPEED) {
            m_board.toggleFlash();
            m_lastTicks = ticks;
        }
    }
    void idleMove() {

    }

    bool containsSquare(int sq,vector<int> squares) {
        for (vector<int>::iterator it = squares.begin() ; it != squares.end(); ++it) {
            if(sq == *it) return true;
        }
        return false;
    }
};

int main(int argc,char* argv[]) {
    printf("You may need to run 'export SDL_VIDEODRIVER=rpi'\n");
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



    int port=9999;
    SockAddr saBind((ULONG)INADDR_ANY,port);
    SimServer server(saBind);
    printf("Server running on port %d\n",port);
    server.runServer();
    printf("Done\n");
    return 0;
}
