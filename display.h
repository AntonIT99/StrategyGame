#ifndef DISPLAY
#define DISPLAY

/*display.h*/

typedef struct SPRITE SPRITE;
struct SPRITE {

    SDL_Surface* surface;

    SDL_Texture* texture;

};

SPRITE init_image(SDL_Renderer *renderer, char *image_name);

SPRITE init_image_unit(SDL_Renderer *renderer, char *image_name);

SPRITE init_image_unit_zoom(SDL_Renderer *renderer, char *image_name);

SPRITE init_image_unit_icon(SDL_Renderer *renderer, char *image_name);

void display_image(SDL_Renderer *renderer, SPRITE image, POINT center);

void display_text(SDL_Renderer *renderer, char* text, char* fontfile, int size, int R, int V, int B, POINT location);

void unload_image(SDL_Renderer *renderer, SPRITE image);

void display_line(SDL_Renderer *renderer, int xA, int yA, int xB, int yB, int Rouge, int Vert, int Bleu, int Alpha);

void display_hexagon(SDL_Renderer *renderer, int xcenter, int ycenter, int Rouge, int Vert, int Bleu, int Alpha);

void display_grid(SDL_Renderer *renderer, int xcorner, int ycorner, int xsize, int ysize);

#endif
