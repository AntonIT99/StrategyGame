#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include "main.h"
#include "game.h"
#include "display.h"
#include "locate.h"
#include "levelmenu.h"

#define true 1
#define false 0


int main(int argc, char** argv) {

    int game_isrunning = true;
    int end = false;

    /// This line is only needed, if you want debug the program
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");

    /****Initialisation****/
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("Strategy Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 900, SDL_WINDOW_OPENGL/*SDL_WINDOW_FULLSCREEN_DESKTOP*/);

    SDL_Renderer *renderer = init_renderer(window);

    SPRITE Background = init_image(renderer, "StrategyGame.jpg");


    POINT p = {100, 300};
    POINT center = {1920/2, 1080/2};
    display_image(renderer, Background, center);
    display_text(renderer, "Strategy Game", "AlteSchwabacher_1.ttf", 256, 255, 255, 255, p);
    SDL_RenderPresent(renderer);

    wait(&end);

    while (!end)
    {
        game_isrunning = menu(window, renderer, game_isrunning, Background);
        if (game_isrunning)
        {
            LEVEL level_to_load = levelmenu(window, renderer, &end);
            if (level_to_load.name != NULL)
            {
                game(window, renderer, level_to_load, &end);
            }
        }
        else
        {
            end = true;
        }
    }

    unload_image(renderer, Background);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

SDL_Renderer *init_renderer(SDL_Window* window)
{
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // permet d'obtenir les redimensionnements plus doux.
    SDL_RenderSetLogicalSize(renderer, 1920, 1080);
    return renderer;
}

void wait(int* endofgame)
{
    int weitergehen = false;
    SDL_Event event;

    while (!weitergehen)
    {
        SDL_WaitEvent(&event);
        //si clique sur la croix
        if (event.type == SDL_QUIT)
        {
            weitergehen = true;
        }
        //si touche appuyée
        if (event.type == SDL_KEYDOWN)
        {
            weitergehen = true;
        }
        //si touche appuyée
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            weitergehen = true;
        }
    }
}
