#include <string>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

const int FULL_SCREEN_MODE = SDL_WINDOW_RESIZABLE;
//const int FULL_SCREEN_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP;
const int SCREEN_WIDTH = 480;  
const int SCREEN_HEIGHT = 800;
#define SCALE 2

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

bool coolSpot() {
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

//    if (!SDL_WasInit(SDL_INIT_JOYSTICK))
//    {
//        printf("initializing joystick %08X", joystick);
//        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
//        int joystickCount = SDL_NumJoysticks();
//        printf("joysticks: %d", joystickCount);
//        joystick = SDL_JoystickOpen(0);
//        if (joystickCount == 2)
//        {
//            SDL_JoystickOpen(1);
//        }
//        SDL_JoystickEventState(SDL_ENABLE);
//    }
//    else
//    {
//        printf("Joystick already initialized");
//    }
//
    window = SDL_CreateWindow("8BitCoder.com",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE);

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
        if (!t) {
            printf("Couldn't initialize SDL: %s\n", SDL_GetError());

        }
        cool[i].setFrame(i % 10);
        x += cool[i].widthSource() * SCALE;
        if (x > SCREEN_WIDTH - cool[i].widthSource()) {
            x = 0;
            y += cool[i].heightSource() * SCALE;
        }
    }

    bool running = true;
    while (running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
                printf("got event type %04X\n", event.type);
                switch (event.type)
                {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    printf("had a window event!!!\n");
                    break;
                case SDL_KEYDOWN:
                    running = false;
                    break;
                default:
                    break;
                }
        }
        SDL_SetRenderDrawColor(renderer, 128, 128, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (int i = 0; i < max; i++)
        {
            cool[i].draw(renderer);
        }

        SDL_Rect rect = { 10,10,50,50 };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderPresent(renderer);

        Uint32 ticks = SDL_GetTicks();
        for (int i = 0; i < max; i++)
        {
            cool[i].update(ticks);
        }
    }
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

}


int main(int argc, char* argv[]) {
    coolSpot();

    return 0;
}