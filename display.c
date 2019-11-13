#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include "main.h"
#include "game.h"
#include "display.h"
#include "locate.h"


void display_image(SDL_Renderer *renderer, SPRITE image, POINT center)
{
    SDL_Rect dest = {center.x - image.surface->w/2,center.y - image.surface->h/2, image.surface->w, image.surface->h};
    SDL_RenderCopy(renderer,image.texture,NULL,&dest);
}

void display_text(SDL_Renderer *renderer, char* text, char* fontfile, int size, int R, int V, int B, POINT location)
{
    char path[200];
    sprintf(path, "fonts/%s", fontfile);
    TTF_Font* font = TTF_OpenFont(path, size);
    SDL_Color color = {R, V, B};
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    int texW = 0, texH = 0;
    SDL_QueryTexture(Message, NULL, NULL, &texW, &texH);
    SDL_Rect p = {location.x, location.y, texW, texH};
    SDL_RenderCopy(renderer, Message, NULL, &p);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
    TTF_CloseFont(font);
}

SPRITE init_image(SDL_Renderer *renderer, char *image_name)
{
    char filename[100] = "assets/";
    strcat(filename, image_name);

    SPRITE image;
    image.surface = IMG_Load(filename);
    image.texture = SDL_CreateTextureFromSurface(renderer,image.surface);

    return image;
}

SPRITE init_image_unit(SDL_Renderer *renderer, char *image_name)
{
    char filename[100] = "assets/units/";
    strcat(filename, image_name);

    SPRITE image;
    image.surface = IMG_Load(filename);
    image.texture = SDL_CreateTextureFromSurface(renderer,image.surface);

    return image;
}

SPRITE init_image_unit_zoom(SDL_Renderer *renderer, char *image_name)
{
    char filename[100] = "assets/units/zoom/";
    strcat(filename, image_name);

    SPRITE image;
    image.surface = IMG_Load(filename);
    image.texture = SDL_CreateTextureFromSurface(renderer,image.surface);

    return image;
}

SPRITE init_image_unit_icon(SDL_Renderer *renderer, char *image_name)
{
    char filename[100] = "assets/units/icons/";
    strcat(filename, image_name);

    SPRITE image;
    image.surface = IMG_Load(filename);
    image.texture = SDL_CreateTextureFromSurface(renderer,image.surface);

    return image;
}

void unload_image(SDL_Renderer *renderer, SPRITE image)
{
    SDL_DestroyTexture(image.texture);
    SDL_FreeSurface(image.surface);
}

void display_line(SDL_Renderer *renderer, int xA, int yA, int xB, int yB, int Rouge, int Vert, int Bleu, int Alpha)
{

    SDL_SetRenderDrawColor(renderer, Rouge, Vert, Bleu, Alpha);
    SDL_RenderDrawLine(renderer, xA, yA, xB, yB);
    SDL_RenderPresent(renderer);
}
void display_hexagon(SDL_Renderer *renderer, int xcenter, int ycenter, int Rouge, int Vert, int Bleu, int Alpha)
{

    SDL_SetRenderDrawColor(renderer, Rouge, Vert, Bleu, Alpha);
    SDL_RenderDrawLine(renderer, xcenter - 50, ycenter + 25, xcenter - 50, ycenter - 25);
    SDL_RenderDrawLine(renderer, xcenter - 50, ycenter + 25, xcenter, ycenter + 50);
    SDL_RenderDrawLine(renderer, xcenter + 50, ycenter + 25, xcenter, ycenter + 50);
    SDL_RenderDrawLine(renderer, xcenter + 50, ycenter + 25, xcenter + 50, ycenter - 25);
    SDL_RenderDrawLine(renderer, xcenter, ycenter - 50, xcenter + 50, ycenter - 25);
    SDL_RenderDrawLine(renderer, xcenter, ycenter - 50, xcenter - 50, ycenter - 25);
    SDL_RenderPresent(renderer);
}
void display_grid(SDL_Renderer *renderer, int xcorner, int ycorner, int xsize, int ysize)
{

    int i, j;
    int y = ycorner + 50;
    int x = xcorner + 50;

    for (i=0;i<xsize;i++) {
        for (j=0;j<ysize;j++) {
            display_hexagon(renderer, x, y, 255, 255, 255, 255);
            display_hexagon(renderer, x + 50, y + 75, 255, 255, 255, 255);
            y = y + 150;

        }
        y = ycorner + 50;
        x = x + 100;

    }
}

