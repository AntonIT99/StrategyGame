/*main.h*/

typedef struct POINT POINT;
struct POINT {

    int x; // Abscisses

    int y; // Ordonnées

};

int main(int argc, char** argv);

SDL_Renderer *init_renderer(SDL_Window* window); ///Initialization

void wait(int* endofgame);


