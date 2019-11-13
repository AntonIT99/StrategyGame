#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <dirent.h>
#include "main.h"
#include "game.h"
#include "display.h"
#include "locate.h"
#include "levelmenu.h"

#define true 1
#define false 0

LEVEL levelmenu(SDL_Window *screen, SDL_Renderer *renderer, int* endofgame)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Event event;

    int menu_isrunning = true;
    int level_id = 0;

    LEVEL level[50];
    LEVEL level_to_load;

    SPRITE level_selection = init_image(renderer, "level_selection.png");

    DIR* dir = NULL;
    struct dirent* file = NULL;

    dir = opendir("levels");

    POINT level_name_location = {100, 250};

    while((file = readdir(dir)) != NULL)
    {
        if (strstr(file->d_name, ".txt") != NULL)
        {
            level_id++;
            level[level_id].name = strdup(strtok(file->d_name, ".")); ///récupère une copie de la valeur du pointeur revoyé par strtok
        }
    }

    while(menu_isrunning)
    {
        display_levelmenu(renderer, level_selection);
        SDL_WaitEvent(&event);

        if (event.type == SDL_QUIT)
        {
            menu_isrunning = false;
            endofgame = true;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    menu_isrunning = false;
                    break;

                case SDLK_F11:
                    SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                    break;

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
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            /*clique gauche*/
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                POINT leftclick = {event.button.x, event.button.y};
                //printf("%d:%d\n", leftclick.x, leftclick.y);

                POINT level_frame_center = {100 + 250, 250 + 36};
                POINT level_frame_leftcorner = get_corner(level_selection, level_frame_center);

                int i;

                for (i = 1; i <= level_id; i++)
                {
                    if (is_included(leftclick.x, level_frame_leftcorner.x, level_frame_leftcorner.x + level_selection.surface->w) && is_included(leftclick.y, level_frame_leftcorner.y + (i - 1)*level_selection.surface->h, level_frame_leftcorner.y + i*level_selection.surface->h))
                    {
                        level_to_load.name = level[i].name;
                        level_to_load = define_level(level_to_load);
                        menu_isrunning = false;

                    }
                }

            }
        }
    }
    unload_image(renderer, level_selection);

    return level_to_load;
}

LEVEL define_level(LEVEL level)
{
    char path[200] = "levels/";
    strncat(path, level.name, 100);
    strncat(path, ".txt", 100);
    FILE* file = fopen(path, "r");

    level.nb_players = 2;

    char line[1000];

    int i = 1;

    ///par défaut on alloue un maximum de 1000 unités dans une partie
    level.unit = malloc(1000*sizeof(UNIT));

    while (fgets(line, 1000, file) != NULL)
    {

        if (strstr(line, "Faction1:") != NULL)
        {
            char* faction1 = strtok(line, ":");
            faction1 = strtok(NULL, ":");
            faction1 = strtok(faction1, "\n");
            level.faction_player[1] = malloc(100*sizeof(char));
            strcpy(level.faction_player[1], faction1);
        }

        if (strstr(line, "Faction2:") != NULL)
        {
            char* faction2 = strtok(line, ":");
            faction2 = strtok(NULL, ":");
            faction2 = strtok(faction2, "\n");
            level.faction_player[2] = malloc(100*sizeof(char));
            strcpy(level.faction_player[2], faction2);
        }

        if (strstr(line, "Unit:") != NULL)
        {
            char line_copy1[200];
            char line_copy2[200];
            char line_copy3[200];
            strcpy(line_copy1, line);
            strcpy(line_copy2, line);
            strcpy(line_copy3, line);

            char* name = strtok(line, ":");
            name = strtok(NULL, ":");
            name = strtok(name, "(");

            level.unit[i].name = malloc(100*sizeof(char));
            strcpy(level.unit[i].name, name);

            char* x = strtok(line_copy1, "(");
            x = strtok(NULL, "(");
            x = strtok(x, ";");

            char* y = strtok(line_copy2, ";");
            y = strtok(NULL, ";");
            y = strtok(y, ";");

            level.unit[i].position.x = atoi(x);
            level.unit[i].position.y = atoi(y);

            char* player = strtok(line_copy3, ";");
            player = strtok(NULL, ";");
            player = strtok(NULL, ";");
            player = strtok(player, ")");

            if (strstr(player, "PLAYER1") != NULL)
            {
                level.unit[i].player = PLAYER1;

            }
            else if (strstr(player, "PLAYER2") != NULL)
            {
                level.unit[i].player = PLAYER2;
            }

            i++;

        }
    }

    level.nb_units = i;

    fclose(file);

    return level;
}

void display_levelmenu(SDL_Renderer *renderer, SPRITE level_selection)
{
    DIR* dir = NULL;
    struct dirent* file = NULL;
    dir = opendir("levels");
    POINT level_name_location = {100, 250};

    while((file = readdir(dir)) != NULL)
    {
        if (strstr(file->d_name, ".txt") != NULL)
        {
            POINT p_level = {level_name_location.x + 250, level_name_location.y + 36};
            char* levelname = strdup(strtok(file->d_name, ".")); ///récupère une copie de la valeur du pointeur revoyé par strtok
            display_image(renderer, level_selection, p_level);
            display_text(renderer, levelname, "arial.ttf", 64, 255, 255, 255, level_name_location);
            SDL_RenderPresent(renderer);
            level_name_location.y += 150;

        }
    }
}
