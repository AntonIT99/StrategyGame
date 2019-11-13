#ifndef LOCATE
#define LOCATE
/*locate*/

typedef struct TILE TILE;
struct TILE {

    int x;

    int y;

};

/*functions to locate*/

POINT get_corner (SPRITE image, POINT center); ///permet d'obtenir les coordonn�es du coin gauche d'une image.

TILE get_tile(SPRITE grid, POINT p, POINT center, SPRITE gridzoom, int zoom); ///permet d'obtenir la case � partir d'un point

TILE get_num_tiles(SPRITE grid, int zoom); ///permet d'obtenir le nombre de cases d'une grille

POINT get_tile_center(SPRITE grid, TILE tile, POINT grid_center, SPRITE gridzoom, int zoom); ///permet d'obtenir le point central d'une case donn�e

int is_included (int number, int inf, int sup); ///v�rifie si un nombre donn� est compris dans un intervalle

int can_reach_tile (TILE origin, TILE target, int range, TILE tile_max); ///v�rifie si une case est � port�e

int click_in_image(SPRITE image, POINT image_location, POINT click); ///v�rifie si on clic sur une image donn�e

#endif
