#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "game.h"
#include "display.h"
#include "locate.h"

#define true 1
#define false 0

POINT get_corner(SPRITE image, POINT center)
{

    POINT corner;
    corner.x = center.x - image.surface->w/2;
    corner.y = center.y - image.surface->h/2;

    return corner;
}

TILE get_tile(SPRITE grid, POINT p, POINT center, SPRITE gridzoom, int zoom)
{
    int tile_size;
    POINT corner;
    TILE tile = {0, 0};
    TILE tile_max;

    if (zoom)
    {
        tile_size = 200;
        corner = get_corner(gridzoom, center);
        tile_max=get_num_tiles(gridzoom, zoom);
    }
    else
    {
        tile_size = 100;
        corner = get_corner(grid, center);
        tile_max=get_num_tiles(grid, zoom);
    }

    int x, y, located = false, nb_hexaX = tile_max.x, nb_hexaY = tile_max.y;

    x = p.x - corner.x;
    y = p.y - corner.y;

    for(tile.x=1; !located && tile.x<=nb_hexaX; tile.x++) //balaie les colonnes
    {
        if ((x>=tile_size*(tile.x-1)) && (x<=tile_size*tile.x)) //si le clic est situé dans une rangée impaire
        {
            int i = 1;
            for(tile.y=1; !located && tile.y<=nb_hexaY; tile.y = tile.y + 2) //balaie les rangées impaires
            {

                //tests correspondant aux équations de droites de l'hexagone
                int test_sup1 = -0.5 * x + (tile_size/4) + (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);
                int test_sup2 = 0.5 * x - (tile_size/4) - (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);
                int test_inf1 = 0.5 * x + (3*tile_size/4) - (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);
                int test_inf2 = -0.5 * x + (5*tile_size/4) + (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);


                if ((y>=test_sup1) && (y>=test_sup2) && (y<=test_inf1) && (y<=test_inf2))
                {
                    located = true;
                    tile.y = tile.y - 2;
                }
                i++;
            }

        }
        if ((x>=tile_size*(tile.x-1) + (tile_size/2)) && (x<=tile_size*tile.x + (tile_size/2))  && !located) { //si le clic est situé dans une rangée paire

            int i = 1;
            for(tile.y=2; !located && tile.y<=nb_hexaY; tile.y = tile.y + 2) //balaie les rangées paires
            {

                //tests correspondant aux équations de droites de l'hexagone
                int test_sup1 = -0.5 * x + (5*tile_size/4) + (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);
                int test_sup2 = 0.5 * x + (tile_size/4) - (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);
                int test_inf1 = 0.5 * x + (5*tile_size/4) - (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);
                int test_inf2 = -0.5 * x + (9*tile_size/4) + (tile_size/2) * (tile.x-1) + (3*tile_size/2) * (i-1);


                if ((y>=test_sup1) && (y>=test_sup2) && (y<=test_inf1) && (y<=test_inf2))
                {
                    located = true;
                    tile.y = tile.y - 2;
                }
                i++;
            }
        }
    }
    tile.x--;

    if (!located)
    {
        tile.x = 0;
        tile.y = 0;
    }

    return tile;
}
TILE get_num_tiles(SPRITE grid, int zoom)
{
    TILE tile_max;
    if (zoom)
    {
        tile_max.x = (grid.surface->w-100)/200;
        tile_max.y = (grid.surface->h-200)/150 + 1;
    }
    else
    {
        tile_max.x = (grid.surface->w-50)/100;
        tile_max.y = (grid.surface->h-100)/75 + 1;
    }
    return tile_max;
}
POINT get_tile_center(SPRITE grid, TILE tile, POINT grid_center, SPRITE gridzoom, int zoom)
{
    int tile_size;

    POINT tile_center;
    tile_center.x = 0;
    tile_center.y = 0;

    POINT corner;

    if (zoom)
    {
        tile_size = 200;
        corner = get_corner(gridzoom, grid_center);
    }
    else
    {
        tile_size = 100;
        corner = get_corner(grid, grid_center);
    }


    if ((tile.x != 0) && (tile.y != 0))
    {
        if ((tile.y%2) == 1) //si coordonnée y impaire
        {
            tile_center.x = tile_size/2 + tile_size * (tile.x - 1);

        } else if (((tile.y%2) == 0) && (tile.y != 0)) { //si coordonnée y paire et non nulle

            tile_center.x = tile_size * tile.x;
        }

        tile_center.y = tile_size/2 + (3*tile_size/4) * (tile.y - 1);

        //ajustement en fonction de la position de la grille
        tile_center.x = tile_center.x + corner.x;
        tile_center.y = tile_center.y + corner.y;
    }

    return tile_center;
}

int is_included (int number, int inf, int sup)
{
    if ((number >= inf) && (number <= sup))
    {
        return true;

    } else {

        return false;
    }
}

int can_reach_tile (TILE origin, TILE target, int range, TILE tile_max)
{
    int tile_can_be_reached = false;
    int n;

    if (is_included(target.x, 1, tile_max.x) && is_included(target.y, 1, tile_max.y))
    {
        if (origin.y%2 == 0) //rangée paire
        {
            for (n = 0; n <= range; n++)
            {
                if ((((target.x == origin.x + range - n) && is_included(target.y, origin.y - (2*n+1), origin.y + (2*n+1)))
                || ((target.x == origin.x - range + n) && is_included(target.y, origin.y - 2*n, origin.y + 2*n)))
                && (fabs(origin.y - target.y) <= range))
                {
                    tile_can_be_reached = true;
                }
            }

        }
        else if (origin.y%2 == 1)  //rangée impaire
        {
            for (n = 0; n <= range; n++)
            {
                if ((((target.x == origin.x + range - n) && is_included(target.y, origin.y - 2*n, origin.y + 2*n))
                || ((target.x == origin.x - range + n) && is_included(target.y, origin.y - (2*n+1), origin.y + (2*n+1))))
                && (fabs(origin.y - target.y) <= range))
                {
                    tile_can_be_reached = true;
                }
            }


        }
    }
    return tile_can_be_reached;
}

int click_in_image(SPRITE image, POINT image_location, POINT click)
{
    int included = false;
    POINT leftcorner = get_corner(image, image_location);

    if (is_included(click.x, leftcorner.x, leftcorner.x + image.surface->w) && is_included(click.y, leftcorner.y, leftcorner.y + image.surface->h))
    {
        included = true;
    }

    return included;
}
