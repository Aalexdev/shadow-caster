#ifndef __MAIN__HPP__
#define __MAIN__HPP__

    class MainVar;

    #include <iostream>
    #include <SDL2/SDL.h>
    using namespace std;
    
    #include <vector>

    #include "light.hpp"

    #define MAX_DELAY 16

    #define WINDOW_WIDTH  1080
    #define WINDOW_HEIGHT 720
    #define WINDOW_AIR    WINDOW_WIDTH * WINDOW_HEIGHT

    

    class MainVar{
        public:
            MainVar(string name, int w, int h);
            ~MainVar();

            void SetWindow(string name);
            void SetWindow(int w, int h);

            bool initRenderer(void);
            bool initWindow(string name, int w, int h);
            bool  initSDL(void);

            SDL_Renderer* getRenderer(void);
            SDL_Window* getWindow(void);

            void quit(void);
            void run(void);

        private:
            SDL_Renderer* renderer;
            SDL_Window* window;

            bool launched;

            bool isInit;

            void event(void);
            void draw(void);
            void update(void);
            void delay(int startTick);

            Light* light;

            void drawRects(void);
    };
#endif