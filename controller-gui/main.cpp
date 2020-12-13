#include <iostream>

#include <SDL.h>
#include <SDL_main.h>

#include "Game.h"
#include "TextureManager.h"
#include "logs.h"

using namespace std;
using namespace ssobjects;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 800;  //16:9 aspect to stay widescreen
const int SCREEN_HEIGHT = 480;
//const int SCREEN_WIDTH = 1024;  //16:9 aspect to stay widescreen
//const int SCREEN_HEIGHT = 768;
SDL_Point pointTopLeft = { 0, 0 };

#define SCALE 1

int jx = 0, jy = 0;

bool handleJoystick(SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_JOYAXISMOTION:
            printf("jaxis axis=%02X value=%04d\n", event->jaxis.axis, event->jaxis.value);
            break;
            //    case SDL_JOYBALLMOTION:   //left/right sticks (thumbstick/joystick) or left/right triggers moved
            //    case SDL_JOYHATMOTION:    //D-pad pressed or released
            //    case SDL_JOYBUTTONDOWN:   //A,B,X,Y,Start,Select,left/right bumper (shoulder),thumbstick button
            //    case SDL_JOYBUTTONUP:     //A,B,X,Y,Start,Select,left/right bumper (shoulder),thumbstick button
            //    case SDL_JOYDEVICEADDED
            //    case SDL_JOYDEVICEREMOVED





        case SDL_JOYDEVICEADDED:
            printf("joystick added\n");
            break;
        case SDL_JOYDEVICEREMOVED:
            printf("joystick removed\n");
            break;
        default:
            return false;
    }
    return true;
}

#define MAX_BUTTONS 10

class Joystick
{
    enum ImageIndex
    {

    };
public:
    Joystick()
    {
        for (int i = 0; i < 4; i++)
        {
            m_hatState = 0;
        }
        for (int i = 0; i < 5; i++)
        {
            m_axis[i] = 0;
        }
    }

    void load(SDL_Renderer* renderer)
    {
        SDL_Point buttonPoints[] = {
                {464,164},
                {502,132},
                {424,127},
                {463,95 },
                {148,56 },
                {414,56 },
                {279,134},
                {365,135},
        };
        SDL_Point dpadPoints[] = {
                {242,196},
                {236,239},
                {214,222},
                {261,233}
        };

        if (!SDL_WasInit(SDL_INIT_JOYSTICK))
        {
            LOG("initializing joystick %08X", m_joystick);
            SDL_InitSubSystem(SDL_INIT_JOYSTICK);
            int joystickCount = SDL_NumJoysticks();
            LOG("joysticks: %d", joystickCount);
            m_joystick = SDL_JoystickOpen(0);
            //if (joystickCount == 2)
            //{
            //    SDL_JoystickOpen(1);
            //}
            //SDL_JoystickEventState(SDL_ENABLE);
        }
        else
        {
            LOG("Joystick already initialized");
        }
        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            m_buttonState[i] = false;
        }

        int i = 0;
        m_texture[i] = loadImage(renderer, "assets/xbox_a.png", &buttonPoints[i], &m_position[i]);
        i++;
        m_texture[i] = loadImage(renderer, "assets/xbox_b.png", &buttonPoints[i], &m_position[i]);
        i++;
        m_texture[i] = loadImage(renderer, "assets/xbox_x.png", &buttonPoints[i], &m_position[i]);
        i++;
        m_texture[i] = loadImage(renderer, "assets/xbox_y.png", &buttonPoints[i], &m_position[i]);
        i++;
        m_texture[i] = loadImage(renderer, "assets/xbox_bumper_left.png", &buttonPoints[i], &m_position[i]);
        i++;
        m_texture[i] = loadImage(renderer, "assets/xbox_bumper_right.png", &buttonPoints[i], &m_position[i]);
        i++;
        m_texture[i] = loadImage(renderer, "assets/xbox_view.png", &buttonPoints[i], &m_position[i]);
        i++;
        m_texture[i] = loadImage(renderer, "assets/xbox_menu.png", &buttonPoints[i], &m_position[i]);
        i++;

        m_textureJoystick = loadImage(renderer, "assets/xbox_controller_white.png", &pointTopLeft, &m_positionJoystick);
    }

    bool handleEvent(SDL_Event* event)
    {
        switch (event->type)
        {
            case SDL_JOYAXISMOTION:
                //LOG("SDL_JOYAXISMOTION axis=%02X value=%04d", event->jaxis.axis, event->jaxis.value);
                m_axis[event->jaxis.axis] = event->jaxis.value;

                ////stick left y=0,x=1,trigger=2 right y=3,x=4,trigger=5
                //if (event->jaxis.axis == 2)
                //{
                //    m_buttonState[0] = event->jaxis.value > -32700;
                //}
                //if (event->jaxis.axis == 5)
                //{
                //    m_buttonState[1] = event->jaxis.value > -32700;
                //}
                break;
                //case SDL_JOYBALLMOTION:
                //    //LOG("SDL_JOYBALLMOTION axis=%02X value=%04d", event->jaxis.axis, event->jaxis.value);
                //    m_axis[event->jaxis.axis] = event->jaxis.value;
                //    break;
                //    case SDL_JOYAXISMOTION:   //left/right
                //    case SDL_JOYBALLMOTION:   //left/right sticks (thumbstick/joystick) or left/right triggers moved
            case SDL_JOYHATMOTION:    //D-pad pressed or released
                //LOG("hat which=%d hat=%d value=%d", event->jhat.which, event->jhat.hat, event->jhat.value);
                m_hatState = event->jhat.value;
                break;
            case SDL_JOYBUTTONDOWN:      //A,B,X,Y,Start,Select,left/right bumper (shoulder),thumbstick button
                m_buttonState[event->jbutton.button] = true;
                break;
            case SDL_JOYBUTTONUP:     //A,B,X,Y,Start,Select,left/right bumper (shoulder),thumbstick button
                m_buttonState[event->jbutton.button] = false;
                break;
                //    case SDL_JOYDEVICEADDED
                //    case SDL_JOYDEVICEREMOVED





            case SDL_JOYDEVICEADDED:
                LOG("joystick added");
                break;
            case SDL_JOYDEVICEREMOVED:
                LOG("joystick removed");
                break;
            default:
                return false;
        }
        return true;
    }

    void draw(SDL_Renderer* renderer)
    {
        drawJoystick(renderer);
        drawHat(renderer);
        drawStick(renderer);
        drawButtons(renderer);
    }

    void drawJoystick(SDL_Renderer* renderer)
    {
        SDL_Rect rect = {0,0,};
        SDL_RenderCopy(renderer, m_textureJoystick, NULL, &m_positionJoystick);
    }

    void drawHat(SDL_Renderer* renderer)
    {
        int move = 22;
        SDL_Rect rect = { 10,10,50,50 };
        SDL_Rect hatRect = { 10+25-3,10+25-3,6,6 };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);

        if (m_hatState & SDL_HAT_UP)
            hatRect.y -= move;
        if (m_hatState & SDL_HAT_DOWN)
            hatRect.y += move;
        if (m_hatState & SDL_HAT_LEFT)
            hatRect.x -= move;
        if (m_hatState & SDL_HAT_RIGHT)
            hatRect.x += move;


        SDL_RenderFillRect(renderer, &hatRect);
    }

    void drawStick(SDL_Renderer* renderer)
    {
        float x = 70, y = 10;
        float w = 50, h = 50;
        SDL_Rect rect = { x,y,w,h };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);

        const float max = 32767;
        float ax = ((float)m_axis[0] / max);
        float ay = ((float)m_axis[1] / max);
        if (ax < -1) ax = -1;
        if (ax > 1) ax = 1;
        if (ay < -1) ay = -1;
        if (ay > 1) ay = 1;
        float xx = (x + w / 2)+20 * ax;
        Sint16 jx = xx;
        float yy = (y + h / 2)+20 * ay;
        Sint16 jy = yy;
        //LOG("ax=%f ay=%f ", ax, ay);
        SDL_Point horz[2] = {
                { jx - 4, jy },
                { jx + 4, jy }
        };
        SDL_Point vert[2] = {
                { jx, jy-4 },
                { jx, jy+4 }
        };
        SDL_RenderDrawLines(renderer, horz, 2);
        SDL_RenderDrawLines(renderer, vert, 2);
    }

    void drawButtons(SDL_Renderer* renderer)
    {
        for (int i = 0; i < MAX_BUTTONS; i++)
        {
            if(m_buttonState[i])
                SDL_RenderCopy(renderer, m_texture[i], NULL, &m_position[i]);
        }
    }

    void close()
    {
        SDL_JoystickClose(m_joystick);
    }

protected:
    SDL_Texture* loadImage(SDL_Renderer* renderer, string filename, SDL_Point* pos, SDL_Rect* rect)
    {
        SDL_Texture* texture = IMG_LoadTexture(renderer, filename.c_str());
        SDL_assert(texture != NULL);
        SDL_QueryTexture(texture, NULL, NULL, &rect->w, &rect->h);
        rect->x = pos->x;
        rect->y = pos->y;
        return texture;
    }
    void renderButton(SDL_Renderer* renderer, int buttonId)
    {
        SDL_Rect rect = { m_position[buttonId].x, m_position[buttonId].y, 0, 0 };
        SDL_QueryTexture(m_texture[buttonId], NULL, NULL, &rect.w, &rect.h);
        rect.x /= 2;
        rect.y /= 2;
        rect.w /= 2;
        rect.h /= 2;
        SDL_RenderCopy(renderer, m_texture[buttonId], NULL, &rect);
    }

    SDL_Texture* m_textureJoystick;
    SDL_Rect m_positionJoystick;
    SDL_Texture* m_texture[MAX_BUTTONS];
    SDL_Rect m_position[MAX_BUTTONS];
    bool m_buttonState[MAX_BUTTONS];
    SDL_Joystick* m_joystick;
    Uint8 m_hatState;
    Sint16 m_axis[10];
};

bool joystickTest()
{
    SDL_Event event;
    Joystick jstick;

    logs::logln("does this work?");


    SDL_EnableScreenSaver();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }


    Uint32 flags = SDL_RENDERER_PRESENTVSYNC;   //you can also use SDL_RENDERER_PRESENTVSYNC to attempt to force sync-to-vblank to reduce tearing
    SDL_Window* window = SDL_CreateWindow("8BitCoder.com",
                                          100,
                                          100,
            //SDL_WINDOWPOS_UNDEFINED,
            //SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_RESIZABLE);
    //SDL_Window* window = SDL_CreateWindow("Coder8Bit.com",
    //    SDL_WINDOWPOS_UNDEFINED,
    //    SDL_WINDOWPOS_UNDEFINED,
    //    0, 0,
    //    SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, flags);
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother
    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    SDL_RenderSetLogicalSize(renderer, 687, 485);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    jstick.load(renderer);

    bool running = true;
    while (running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            //if (event.type != SDL_JOYAXISMOTION)
            //    LOG("got event type %04X", event.type);
            if (!jstick.handleEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_WINDOWEVENT:
                        //LOG("had a window event");
                        break;
                    case SDL_KEYUP:                                   //if escape key was pressed, we quit our demo
                        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                            running = false;
                        break;
                    default:
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        jstick.draw(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    //jstick.close();

    SDL_Quit();
}

class AnimSprite
{
public:
    AnimSprite() {}
    SDL_Texture* load(SDL_Renderer* renderer, string filename, int frameCount, int x, int y)
    {
        m_texture = IMG_LoadTexture(renderer, filename.c_str());
        m_delay = 100;

        m_currentFrame = 0;
        m_sourceRect.x = 0;
        m_sourceRect.y = 0;
        SDL_QueryTexture(m_texture, NULL, NULL, &m_sourceRect.w, &m_sourceRect.h);
        m_sourceRect.w /= frameCount;   //each frame of the animation
        m_frameCount = frameCount;
        m_destinationRect.x = x;
        m_destinationRect.y = y;
        m_destinationRect.w = m_sourceRect.w * SCALE;
        m_destinationRect.h = m_sourceRect.h * SCALE;
        return m_texture;
    }
    void draw(SDL_Renderer* renderer)
    {
        SDL_RenderCopyEx(renderer, m_texture, &m_sourceRect, &m_destinationRect, 0, 0, SDL_FLIP_NONE);
    }
    void update(int ticks)
    {
        m_sourceRect.x = m_sourceRect.w * int(((ticks / m_delay) % m_frameCount));

    }
    void setPos(int x, int y)
    {
        m_destinationRect.x = x;
        m_destinationRect.y = y;
    }
    void setFrame(int frame)
    {
        m_currentFrame = frame;
    }
    void setDelay(Uint32 delayMilliSeconds)
    {
        m_delay = delayMilliSeconds;
    }
    int widthDest() { return m_destinationRect.w; }
    int heightDest() { return m_destinationRect.h; }
    int widthSource() { return m_sourceRect.w; }
    int heightSource() { return m_sourceRect.h; }
protected:
    SDL_Texture* m_texture;
    int m_frameCount;
    int m_currentFrame;
    Uint32 m_delay;
    SDL_Rect m_sourceRect;
    SDL_Rect m_destinationRect;
};

bool coolSpot()
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Event event;
    SDL_Rect r;
    SDL_Rect r2;
    SDL_Point center;
    SDL_Joystick* joystick = nullptr;

    SDL_EnableScreenSaver();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_WasInit(SDL_INIT_JOYSTICK))
    {
        printf("initializing joystick %08X\n",joystick);
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
        int joystickCount = SDL_NumJoysticks();
        printf("joysticks: %d\n", joystickCount);
        joystick = SDL_JoystickOpen(0);
        if (joystickCount == 2)
        {
            SDL_JoystickOpen(1);
        }
        SDL_JoystickEventState(SDL_ENABLE);
    }
    else
    {
        printf("Joystick already initialized\n");
    }

    window = SDL_CreateWindow("8BitCoder.com",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              FULL_SCREEN_MODE);

    r.w = 100;
    r.h = 50;
    r2.w = r.w;
    r2.h = r.h;
    center.x = r.w / 2;
    center.y = r.h / 2;

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);

    const int max = 25;
    AnimSprite cool[max];
    int x = 0;
    int y = 0;
    for (int i = 0; i < max; i++)
    {
        SDL_Texture* t = cool[i].load(renderer, "coolspot_fingersnap.png", 10, x, y);
//        SDL_Texture* t = cool[i].load(renderer, "C:/workspace/chessbox/controller-gui/assets/coolspot_fingersnap.png", 10, x, y);
        if(!t) {
            printf("unable to load texture\n");
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize image: %s", SDL_GetError());
        }
        cool[i].setFrame(i % 10);
        x += 48*SCALE + 10;
        if (x > 1024 - 60) {
            x = 0;
            //y += cool[i].heightDest();
            y += 32*SCALE;
        }
    }
    bool running = true;
    while (running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if(event.type != SDL_JOYAXISMOTION)
                printf("got event type %04X", event.type);
            if (!handleJoystick(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_WINDOWEVENT:
                        printf("had a window event");
                        break;
                    case SDL_KEYDOWN:
                        running = false;
                        break;
                    default:
                        break;
                }
            }
        }
        SDL_RenderClear(renderer);
        SDL_Rect rect = { 10,10,50,50 };
        SDL_Rect hatRect = { 10+25-3,10+25-3,6,6 };

        for (int i = 0; i < max; i++)
        {
            cool[i].draw(renderer);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderPresent(renderer);

        //update frame
        Uint32 ticks = SDL_GetTicks();
        for (int i = 0; i < max; i++)
        {
            cool[i].update(ticks);
        }
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    if(joystick)
        SDL_JoystickClose(joystick);

    SDL_Quit();
    return true;
}


int main(int argc,char* argv[]) {
//    joystickTest();
    coolSpot();
    return 0;
}
