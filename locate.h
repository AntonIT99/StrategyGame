#ifndef LOCATE
#define LOCATE
/*locate*/

typedef struct TILE TILE;
struct TILE {

    int x;

    int y;

};

/*functions to locate*/

POINT get_corner (SPRITE image, POINT center); ///permet d'obtenir les coordonnées du coin gauche d'une image.

TILE get_tile(SPRITE grid, POINT p, POINT center, SPRITE gridzoom, int zoom); ///permet d'obtenir la case à partir d'un point

TILE get_num_tiles(SPRITE grid, int zoom); ///permet d'obtenir le nombre de cases d'une grille

POINT get_tile_center(SPRITE grid, TILE tile, POINT grid_center, SPRITE gridzoom, int zoom); ///permet d'obtenir le point central d'une case donnée

int is_included (int number, int inf, int sup); ///vérifie si un nombre donné est compris dans un intervalle

int can_reach_tile (TILE origin, TILE target, int range, TILE tile_max); ///vérifie si une case est à portée

int click_in_image(SPRITE image, POINT image_location, POINT click); ///vérifie si on clic sur une image donnée

#endif
