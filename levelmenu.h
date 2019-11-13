/*levelmenu.h*/

#ifndef LEVELMENU
#define LEVELMENU


LEVEL levelmenu(SDL_Window *screen, SDL_Renderer *renderer, int* endofgame);

LEVEL define_level(LEVEL level);

void display_levelmenu(SDL_Renderer *renderer, SPRITE level_selection);

#endif
