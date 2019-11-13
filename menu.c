#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>
#include "main.h"
#include "game.h"
#include "display.h"
#include "locate.h"

#define true 1
#define false 0

int menu(SDL_Window *screen, SDL_Renderer *renderer, int game_isrunning, SPRITE Background);

int menu(SDL_Window *screen, SDL_Renderer *renderer, int game_isrunning, SPRITE Background)
{
    int menu_isrunning = true;
    int hover = false;
    int hoverquit = false;
    /*int Mix_OpenAudio(int 44100,Uint16,int 10, int 1024);
    Mix_Music *musique;*/

    ///chargement des images
    SPRITE play_button = init_image(renderer, "jouer.png");
    SPRITE quit_button = init_image(renderer, "quitter.png");
    SPRITE pressed_play_button = init_image(renderer, "jouerpressed.png");
    SPRITE pressed_quit_button = init_image(renderer, "quitterpressed.png");
    SPRITE hover_play_button = init_image(renderer, "jouerhover.png");
    SPRITE hover_quit_button = init_image(renderer, "quitterhover.png");

    POINT p = {0, 0};
    POINT center = {1920/2, 1080/2};
    POINT p_play = {1920/2, 500};
    POINT p_quit = {1920/2, 700};
    POINT ph;

    SDL_Event event;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    display_image(renderer, Background, center);
    SDL_RenderPresent(renderer);

    while(menu_isrunning)
    {
        //musique = Mix_LoadMUS("musique.mp3")
        //Mix_PlayingMusic(musique, -1);

        if (hover)
        {
            display_image(renderer, hover_play_button, p_play);

        } else {

            display_image(renderer, play_button, p_play);

        }

        if (hoverquit)
        {
            display_image(renderer, hover_quit_button, p_quit);

        } else {

            display_image(renderer, quit_button, p_quit);

        }

        SDL_RenderPresent(renderer);
        SDL_WaitEvent(&event);

        /*si clique sur la croix*/
        if (event.type == SDL_QUIT)
        {
            game_isrunning = false;
            menu_isrunning = false;
        }
        /*si touche appuyée*/
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    game_isrunning = false;
                    menu_isrunning = false;
                    break;

                //plein écran
                case SDLK_F11:
                    SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                    break;

                //mode fenêtre
                case SDLK_F9:
                    SDL_SetWindowFullscreen(screen, 0);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                    break;

                default:
                    break;
            }
        }

        if (event.type == SDL_MOUSEMOTION)
        {
                ph.x = event.motion.x;
                ph.y = event.motion.y;
        }

        /*si clique souris*/
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                p.x = event.button.x;
                p.y = event.button.y;
            }
        }
        //bouton play
        if ((p.x >= p_play.x - 200)&&(p.x <= p_play.x + 200) && (p.y >= p_play.y - 50)&&(p.y <= p_play.y + 50)) {
            display_image(renderer, pressed_play_button, p_play);
            SDL_RenderPresent(renderer);
            SDL_Delay(250);
            menu_isrunning = false;
        }
        if ((ph.x >= p_play.x - 200)&&(ph.x <= p_play.x + 200) && (ph.y >= p_play.y - 50)&&(ph.y <= p_play.y + 50)) {

            hover = true;

        } else {

            hover = false;

        }
        if ((ph.x >= p_quit.x - 200)&&(ph.x <= p_quit.x + 200) && (ph.y >= p_quit.y - 50)&&(ph.y <= p_quit.y + 50)) {

            hoverquit = true;

        } else {

            hoverquit = false;

        }
        //bouton quitter
        if ((p.x >= p_quit.x - 200)&&(p.x <= p_quit.x + 200) && (p.y >= p_quit.y - 50)&&(p.y <= p_quit.y + 50)) {
            display_image(renderer, pressed_quit_button, p_quit);
            SDL_RenderPresent(renderer);
            SDL_Delay(250);
            game_isrunning = false;
            menu_isrunning = false;
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    //Mix_FreeMusic(musique);
    //Mix_CloseAudio();
    unload_image(renderer, play_button);
    unload_image(renderer, pressed_play_button);
    unload_image(renderer, hover_play_button);
    unload_image(renderer, quit_button);
    unload_image(renderer, pressed_quit_button);
    unload_image(renderer, hover_quit_button);

    return game_isrunning;
}
