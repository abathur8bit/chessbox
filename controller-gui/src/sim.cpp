#include <SDL.h>
#include <stdio.h>
#include <ctype.h>
#include <vector>
#include "telnetserver.h"
#include "json.hpp"
#include "Window.h"
#include "FontManager.h"

#ifdef WIN32
WSADATA m_wsd;              ///< WSA startup information (Windows only)
#endif

using namespace nlohmann;   //json
using namespace std;

#define SCREEN_WIDTH    400
#define SCREEN_HEIGHT   400
#define NUM_SQUARES     64

class SimBoard : public Component {
protected:
    SDL_Rect m_rectSquare;
    SDL_Color m_blackColor;
    SDL_Color m_whiteColor;
    bool m_flashState = false;
    bool m_led[NUM_SQUARES];
    bool m_pieces[NUM_SQUARES];
    bool m_flash[NUM_SQUARES];
    const char m_fen[NUM_SQUARES][3] = {
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

    const char* san(int x, int y) {
        return m_fen[y*8+x];
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
    void setFlash(int n,bool state) {
        m_flash[n] = state;
    }
    bool flash(int n) {
        return m_flash[n];
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

class Sim : public TelnetServer,Window {
protected:
    char m_buffer[1024];
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SimBoard m_board;
    long m_lastTicks;
public:
    Sim(const SockAddr& saBind) :
            TelnetServer(saBind),
            Window("chessbox sim",0,0,SCREEN_WIDTH,SCREEN_HEIGHT),
            m_window(nullptr),
            m_renderer(nullptr),
            m_board(0, 0, SCREEN_WIDTH, SCREEN_WIDTH),
            m_lastTicks(0)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
            return;
        }
        m_window = SDL_CreateWindow(
                "Chessbox Simulator",
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                50,50,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_RESIZABLE);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        FontManager::instance()->add("small","Inconsolata-Medium.ttf",10);
        FontManager::instance()->add("normal","Inconsolata-Medium.ttf",16);
        FontManager::instance()->add("large","Inconsolata-Medium.ttf",26);
        m_renderer = SDL_CreateRenderer(m_window, -1, 0);
        if(!m_renderer) {
            printf("m_renderer error %s\n",SDL_GetError());
        }
        Sprite* logo=new Sprite("logo");
        logo->load(m_renderer, "assets/logo-sm.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        addComponent(logo);

        int x=0,y=480,w=60,h=60,gap=5;
        addComponent(&m_board);
        TextButton* level=new TextButton("levelbutton","Close",x,y,w,h);
        addButton(level);

        draw(m_renderer);
        SDL_RenderPresent(m_renderer);
    }
    ~Sim() {
        SDL_DestroyWindow(m_window);
        SDL_DestroyRenderer(m_renderer);
        SDL_Quit();
    }
    virtual void connected(SocketInstance sock,SockAddr& sa) {
        printf("Connection from %s opened\n",sa.dottedDecimal());
        json j;
        j["message"] = "Hello";
        println(sock, j.dump().c_str());
    }
    virtual void closed(SocketInstance sock,SockAddr& sa) {
        printf("Connection from %s closed\n",sa.dottedDecimal());
    }
    virtual void processLine(SocketInstance sock,SockAddr& sa,char* line) {
        printf("Got [%s] from client\n",line);
        try {
            json j=json::parse(line);
            if(j.contains("action")) {
                json jresult;
                jresult["success"]=false;

                string action=j["action"];
                if(action.compare("quit")==0) {
                    jresult["success"]=true;
                    jresult["message"]="Good bye";
                    println(sock, jresult.dump().c_str());
                    sock.close();
                } else if(action.compare("led")==0) {
                    bool on=j["on"];
                    auto squares = j["squares"];
                    for(int i=0; i<squares.size(); i++) {
                        int sq=squares.at(i);
                        m_board.setLed(sq, on);
                    }
                    jresult["success"]=true;
                } else if(action.compare("led_all")==0) {
                    bool on=j["on"];
                    m_board.setLedAll(on);
                    jresult["success"]=true;
                } else if(action.compare("flash")==0) {
                    bool on=j["on"];
                    auto squares=j["squares"];
                    for(int i=0; i<squares.size(); i++) {
                        int sq=squares.at(i);
                        m_board.setFlash(sq,on);
                    }
                } else if(action.compare("query_leds")==0) {
                    json leds = json::array({});
                    for(int i=0; i<NUM_SQUARES; i++) {
                        if(m_board.led(i)) {
                            leds.push_back(i);
                        }
                    }
                    jresult["leds_on"] = leds;
                    jresult["success"] = true;
                } else if(action.compare("query_pieces")==0) {
                    json switches = json::array({});
                    for(int i=0; i<NUM_SQUARES; i++) {
                        if(m_board.piece(i)) {
                            switches.push_back(i);
                        }
                    }
                    jresult["has_piece"] = switches;
                    jresult["success"] = true;
                }
                println(sock,jresult.dump().c_str());
            } else {
                json j;
                j["error"] = "missing action";
                println(sock,j.dump().c_str());
            }
        } catch (json::exception& ex) {
            printf("parse error at byte %s\n",ex.what());
            json j;
            j["error"] = "invalid json";
            println(sock,j.dump().c_str());
            sock.close();
        }

    }
    void mouse(SDL_Event* event) {
        printf("button x=%d y=%d\n",event->button.x,event->button.y);
        if(m_sClient != 0) {
            int w=SCREEN_WIDTH / 8;
            int h=SCREEN_HEIGHT / 8;
            int x=event->button.x / w;
            int y=event->button.y / h;
            m_board.setPiece(x, y, !m_board.piece(x, y));
            json j;
            j["success"]=true;
            j["action"]=m_board.piece(x, y) ? "piece_down" : "piece_up";
            j["square"]=y * 8 + x;
            j["san"] =m_board.san(x, y);
            println(m_sClient, j.dump().c_str());
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

    }
    virtual void update(long ticks) {
        Window::update(ticks);
        long delta = ticks-m_lastTicks;
        if(delta>=100) {
            m_board.toggleFlash();
        }
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
    Sim server(saBind);
    server.runServer();
    printf("Done\n");
    return 0;
}
