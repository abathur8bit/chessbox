void svgtest(const char* filename,bool fullscreen) {
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_running;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return;
    }
    m_window = SDL_CreateWindow(
            "Chessbox",
            300,100,
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP:SDL_WINDOW_RESIZABLE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if(!m_renderer) {
        printf("m_renderer error %s\n",SDL_GetError());
    }

    SDL_RWops *src = SDL_RWFromFile(filename, "rb");
    SDL_Surface * surface = IMG_LoadSVG_RW(src);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer,surface);
    SDL_Rect srcrect={0,0,0,0};
    SDL_QueryTexture(texture, NULL, NULL, &srcrect.w, &srcrect.h);
    SDL_Rect destrect={0,0,srcrect.w,srcrect.h};

    bool running=true;
    while(running) {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_WINDOWEVENT:
                    break;
                case SDL_KEYDOWN:
                    if(SDL_SCANCODE_ESCAPE==event.key.keysym.scancode)
                        running = false;
                    break;
                default:
                    break;
            }
        }
        SDL_SetRenderDrawColor(m_renderer, 128, 0, 0, 255);
        SDL_RenderClear(m_renderer);
        SDL_RenderCopyEx(m_renderer,texture, &srcrect, &destrect, 0, 0, SDL_FLIP_NONE);
        SDL_RenderPresent(m_renderer);
    }
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    SDL_Quit();


}

int main(int argc, char* argv[]) {
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

    bool fullscreen=false;
    for(int i=1; i<argc; i++) {
        if(!strcmp("-f",argv[i])) {
            fullscreen=true;
        }
    }
    svgtest("assets/chessbox-box.svg",false);
    int err=system(NULL);
    printf("system returned %d\n",err);
    return 0;
}