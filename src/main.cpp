#include "main.hpp"

int main(int argc, char** argv){

    static MainVar* mainVar = new MainVar("light tracker", 1080, 720);


    mainVar->run();
    mainVar->quit();

    delete mainVar;

    return 0;
}

void MainVar::run(void){
    while (launched){
        int tick = SDL_GetTicks();
        event();
        update();
        draw();
        delay(tick);
    }
}

MainVar::MainVar(string name, int w, int h){
    if (initSDL() && initWindow(name, w, h) && initRenderer()){
        launched = true;
    } else {
        launched = false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_MOD);
    
    light = new Light(this);
    light->pushRect(1, 1, WINDOW_WIDTH-3, WINDOW_HEIGHT-3);
    light->pushRectF(200, 200, 200, 20);
    light->pushRectF(200, 200, 20, 200);

    light->convertTileMapToPolyMap(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH);
}

MainVar::~MainVar(){

    delete light;
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void MainVar::SetWindow(string name){
    if (!window){
        cerr << "setWindow : cannot set a null window attribute" << endl;
    }

    SDL_SetWindowTitle(window, name.c_str());
}
void MainVar::SetWindow(int w, int h){
    if (!window){
        cerr << "setWindow : cannot set a null window attribute" << endl;
    }

    SDL_SetWindowSize(window, w, h);
}

bool MainVar::initRenderer(void){
    if (renderer) SDL_DestroyRenderer(renderer);
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer){
        cerr << SDL_GetError() << endl;
        return false;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    return true;
}
bool MainVar::initWindow(string name, int w, int h){
    if (window) SDL_DestroyWindow(window);
    window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);

    if (!window){
        cerr << SDL_GetError() << endl;
        return false;
    }
    return true;
}

SDL_Renderer* MainVar::getRenderer(void){
    return renderer;
}

SDL_Window* MainVar::getWindow(void){
    return window;
}

void MainVar::quit(void){
    launched = false;
}

void MainVar::event(void){
    SDL_Event e;

    if (SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT:
                quit();
                break;
            
            case SDL_MOUSEMOTION:
                light->clearLight();
                light->pushLightSource(e.motion.x, e.motion.y);
                break;
            
            default:
                break;
        }
    }
}

void MainVar::draw(void){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    if (!light->draw()) quit();

    SDL_RenderPresent(renderer);
}

void MainVar::update(void){

}

bool MainVar::initSDL(void){
    if (SDL_Init(SDL_INIT_VIDEO || SDL_INIT_HAPTIC)){
        cerr << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void MainVar::delay(int startTick){
    int finnalTick = SDL_GetTicks();

    int execTime = (finnalTick - startTick) > MAX_DELAY ? MAX_DELAY : (finnalTick - startTick);
    SDL_Delay(MAX_DELAY - execTime);
}