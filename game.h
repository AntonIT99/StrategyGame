/*game.h*/

#ifndef GAME
#define GAME

#include "display.h"
#include "locate.h"

typedef enum PLAYER PLAYER;
enum PLAYER {
    PLAYER1, PLAYER2, PLAYER3, PLAYER4
};

typedef enum LAND LAND;
enum LAND {
    ROAD, PLAINS, FOREST, WATER, OBSTACLE
};

typedef enum UNITTYPE UNITTYPE;
enum UNITTYPE {
    VEHICLE, INFANTRY, PLANE, OTHER
};

typedef struct UNITSTATS UNITSTATS;
struct UNITSTATS {

    int life_base;
    int attack_base;
    int attack_infantry;
    int attack_vehicle;
    int attack_plane;
    int resistance;
    int range;
    int movement_base;
};


typedef struct UNIT UNIT;
struct UNIT {

    char* name;
    char* id;
    UNITTYPE type;
    PLAYER player;
    SPRITE image;
    SPRITE imagezoom;
    SPRITE icon;
    TILE position;
    UNITSTATS stats;
    int life;
    int can_attack;
    int can_retaliate;
    int movement;
};



typedef struct LEVEL LEVEL;
struct LEVEL {

    char* name;
    UNIT* unit;
    char* faction_player[10];
    int nb_units;
    int nb_players;

};

typedef struct GROUND GROUND;
struct GROUND {

    LAND type;
    int movement_modifier;
    int resistance_modifier;
};

/*functions*/

void init_game(UNIT unit[], SPRITE unit_sprite[], SPRITE unit_sprite_zoom[], SPRITE unit_sprite_icon[], LEVEL level);

UNIT unit_attack(UNIT unit1, UNIT unit2, int zoom, SDL_Renderer *renderer, SPRITE grid, SPRITE gridzoom, POINT grid_center, SPRITE explosion, SPRITE explosionzoom);
///fonction d'attaque

void game(SDL_Window *screen, SDL_Renderer *renderer, LEVEL level, int* endofgame);

#endif
