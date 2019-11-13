#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <dirent.h>
#include "main.h"
#include "game.h"
#include "display.h"
#include "locate.h"
#include "defineunits.h"
#include "levelmenu.h"

#define true 1
#define false 0

void game(SDL_Window *screen, SDL_Renderer *renderer, LEVEL level, int* endofgame)
{
    int i;
    int selected_unit = -1; //indice de la dernière unité selectionnée
    int attacked_unit = -1; //indice de la dernière unité attaquée
    int nb_units = 0; //nombre d'unités sur le terrain
    int nb_units_team1 = 0; //nombre d'unités de l'équipe 1
    int nb_units_team2 = 0; //nombre d'unités de l'équipe 2
    //booleans
    int game_isrunning = true;
    int mouserightbutton_ispressed = false;
    int unit_is_selected = false;
    int click_end_turn = false;
    int zoom = false;
    int attack_occured = false; //true juste après une attaque


    ///center is the center of the screen
    ///p is the real position of the center of the grid
    ///click is the position of the click
    POINT p, click;
    p.x = 1920/2;
    p.y = 1080/2;

    POINT center;
    center.x = 1920/2;
    center.y = 1080/2;

    POINT button_end_turn = {1845, 1005};
    POINT button_zoom = {75, 1005};

    POINT selected;
    TILE tile;

    SDL_Event event;

    SPRITE Faction[level.nb_players];
    for (i = 1; i<=level.nb_players; i++)
    {
        char path[100] = "factions/";
        strcat(strcat(path, level.faction_player[i]), ".png");
        Faction[i] = init_image(renderer, path);
    }

    ///chargement des textures
    SPRITE grid = init_image(renderer, "levels/huertgendwald.png");
    SPRITE background = init_image(renderer, "background.jpg");
    SPRITE gridzoom = init_image(renderer, "levels/zoom/huertgendwald.png");
    SPRITE selection = init_image(renderer, "selection.png");
    SPRITE selectionzoom = init_image(renderer, "zoom/selection.png");
    SPRITE movement = init_image(renderer, "movement.png");
    SPRITE movementzoom = init_image(renderer, "zoom/movement.png");
    SPRITE attack = init_image(renderer, "attack.png");
    SPRITE attackzoom = init_image(renderer, "zoom/attack.png");
    SPRITE explosion = init_image(renderer, "explosion.png");
    SPRITE explosionzoom = init_image(renderer, "zoom/explosion.png");
    SPRITE lifebar = init_image(renderer, "lifebar.png");
    SPRITE damagebar = init_image(renderer, "damagebar.png");

    ///chargement du Graphical User Interface
    SPRITE ingame_menu = init_image(renderer, "gui/ingame_menu.png");
    SPRITE main_menu = init_image(renderer, "gui/menu principal.png");
    SPRITE quit = init_image(renderer, "gui/quitter.png");
    SPRITE backtogame = init_image(renderer, "gui/reprendre.png");
    SPRITE savegame = init_image(renderer, "gui/sauvegarder.png");
    SPRITE zoomin = init_image(renderer, "gui/zoom+.png");
    SPRITE zoominpressed = init_image(renderer, "gui/zoom+pressed.png");
    SPRITE zoomout = init_image(renderer, "gui/zoom-.png");
    SPRITE zoomoutpressed = init_image(renderer, "gui/zoom-pressed.png");
    SPRITE endturn = init_image(renderer, "gui/EndTurn.png");
    SPRITE endturnpressed = init_image(renderer, "gui/EndTurn_pressed.png");
    SPRITE unit_screen = init_image(renderer, "gui/unit_screen.png");

    ///chargement des images associées aux différentes unités
    DIR* dir = NULL;
    struct dirent* file = NULL;
    int nb_images = 0;

    dir = opendir("assets/units");

    while((file = readdir(dir)) != NULL)
    {
        if (strstr(file->d_name, ".png") != NULL)
        {
            nb_images++;
        }
    }

    closedir(dir);

    SPRITE unit_sprite[nb_images + 1];
    SPRITE unit_sprite_zoom[nb_images + 1];
    SPRITE unit_sprite_icon[nb_images + 1];
    int image_id = 0;
    dir = opendir("assets/units");

    while((file = readdir(dir)) != NULL)
    {
        if (strstr(file->d_name, ".png") != NULL)
        {
                unit_sprite[image_id] = init_image_unit(renderer, file->d_name);
                unit_sprite_zoom[image_id] = init_image_unit_zoom(renderer, file->d_name);
                unit_sprite_icon[image_id] = init_image_unit_icon(renderer, file->d_name);
                image_id++;
        }
    }
    closedir(dir);

    ///calcul de la taille de la grille
    TILE tile_max = get_num_tiles(grid, zoom);

    ///initialisation de la partie et chargement des unités
    UNIT unit[1000];
    init_game(unit, unit_sprite, unit_sprite_zoom, unit_sprite_icon, level);

    nb_units = level.nb_units - 1;

    for (i=1; i<=nb_units; i++)
    {
        //initialisation du nombre d'unités par équipe
        if ((unit[i].player == PLAYER1) || (unit[i].player == PLAYER3))
        {
            nb_units_team1++;
        }
        else if ((unit[i].player == PLAYER2) || (unit[i].player == PLAYER4))
        {
            nb_units_team2++;
        }

        //initialisation de la capacité à attaquer et à contre-attaquer
        unit[i].can_attack = true;
        unit[i].can_retaliate = true;
    }


    PLAYER current_player = PLAYER1;
    display_image(renderer, Faction[1], center);
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);


    while(game_isrunning)
    {
            ///si contre-attaque possible
            ///contre-attaque de l'unité 2 si possible
            if (attack_occured && unit[attacked_unit].can_retaliate)
            {
                unit[selected_unit] = unit_attack(unit[attacked_unit], unit[selected_unit], zoom, renderer, grid, gridzoom, p, explosion, explosionzoom);
                unit[attacked_unit].can_retaliate = false;

                ///si l'unité est détruite lors de la contre-attaque, soustraire 1 au nombre d'unités alliées et déselectionner l'unité
                if (unit[selected_unit].life <= 0)
                {
                    if ((unit[selected_unit].player == PLAYER1) || (unit[selected_unit].player == PLAYER3))
                    {
                        nb_units_team1--;
                    }
                    else if ((unit[selected_unit].player == PLAYER2) || (unit[selected_unit].player == PLAYER4))
                    {
                        nb_units_team2--;
                    }
                    unit_is_selected = false;
                }
            }
            attack_occured = false;


            while(SDL_PollEvent(&event) != 0)
            {
                /*si clique sur la croix*/
                if (event.type == SDL_QUIT)
                {
                    game_isrunning = false;
                }
                /*si touche appuyée*/
                if (event.type == SDL_KEYDOWN)
                {

                    switch (event.key.keysym.sym)
                    {

                        case SDLK_ESCAPE:
                            display_image(renderer, ingame_menu, center);
                            POINT button_location1 = {960, 365};
                            POINT button_location2 = {960, 515};
                            POINT button_location3 = {960, 665};
                            POINT button_location4 = {960, 815};
                            display_image(renderer, backtogame, button_location1);
                            display_image(renderer, savegame, button_location2);
                            display_image(renderer, main_menu, button_location3);
                            display_image(renderer, quit, button_location4);
                            SDL_RenderPresent(renderer);
                            int pause = true;
                            while (pause)
                            {
                                while(SDL_PollEvent(&event) != 0)
                                {
                                    if (event.type == SDL_MOUSEBUTTONDOWN)
                                    {
                                        if (event.button.button == SDL_BUTTON_LEFT)
                                        {
                                            POINT button_leftcorner = get_corner(backtogame, button_location1);
                                            int n, button = 0;
                                            for (n = 1; n<=4; n++)
                                            {
                                                if (is_included(event.button.x, button_leftcorner.x, button_leftcorner.x + backtogame.surface->w) && is_included(event.button.y, button_leftcorner.y + (n-1)* 150, button_leftcorner.y + backtogame.surface->h + (n-1)* 150))
                                                {
                                                    button = n;
                                                }
                                            }
                                            switch (button)
                                            {
                                                case 0:
                                                    break;

                                                case 1://reprendre le jeu
                                                   pause = false;
                                                   break;

                                                case 2://sauvegarder
                                                    break;

                                                case 3://menu principal
                                                    pause = false;
                                                    game_isrunning = false;
                                                    break;

                                                case 4://quitter
                                                    pause = false;
                                                    game_isrunning = false;
                                                    *endofgame = true;
                                                    break;
                                            }

                                        }
                                    }
                                    if (event.type == SDL_KEYDOWN)
                                    {
                                        if (event.key.keysym.sym == SDLK_ESCAPE)
                                        {
                                            pause = false;
                                        }
                                    }
                                    if (event.type == SDL_QUIT)
                                    {
                                        pause = false;
                                        game_isrunning = false;
                                        *endofgame = true;
                                    }
                                }
                            }
                            break;

                        case SDLK_F11:
                            SDL_SetWindowFullscreen(screen, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            break;

                        case SDLK_F9:
                            SDL_SetWindowFullscreen(screen, 0);
                            break;
                    }
                }
                if (event.type == SDL_MOUSEBUTTONUP)
                {
                    mouserightbutton_ispressed = false;

                }
                /*si clique souris*/
                if (event.type == SDL_MOUSEBUTTONDOWN)
                {
                    /*clique gauche*/
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        click.x = event.button.x;
                        click.y = event.button.y;

                        if (click_in_image(endturn, button_end_turn, click))
                        {
                            display_image(renderer, endturnpressed, button_end_turn);
                            SDL_RenderPresent(renderer);
                            SDL_Delay(500);
                            click_end_turn = true;

                        }
                        else if (click_in_image(zoomin, button_zoom, click))
                        {
                            if (zoom == true)
                            {
                                display_image(renderer, zoomoutpressed, button_zoom);
                                zoom = false;
                            }
                            else
                            {
                                display_image(renderer, zoominpressed, button_zoom);
                                zoom = true;
                            }
                            SDL_RenderPresent(renderer);
                            SDL_Delay(500);

                        }
                        else
                        {

                            ///case cliquée
                            tile = get_tile(grid, click, p, gridzoom, zoom);

                            ///attaque
                            if ((event.button.clicks == 2) && (unit_is_selected) && can_reach_tile(unit[selected_unit].position, tile, unit[selected_unit].stats.range, tile_max) && unit[selected_unit].can_attack)
                            {
                                for (i=1; i<=nb_units; i++)
                                {
                                    if ((unit[i].position.x == tile.x) && (unit[i].position.y == tile.y))
                                    {
                                        if ((unit[i].player != unit[selected_unit].player) && (unit[i].player != unit[selected_unit].player + 2) && (unit[i].player != unit[selected_unit].player - 2))
                                        {
                                            attacked_unit = i;
                                            unit[attacked_unit] = unit_attack(unit[selected_unit], unit[attacked_unit], zoom, renderer, grid, gridzoom, p, explosion, explosionzoom);
                                            attack_occured = true;
                                            unit[selected_unit].can_attack = false;

                                            ///si l'unité ennemie est détruite, soustraire 1 au nombre d'unités adverses
                                            if (unit[attacked_unit].life <= 0)
                                            {
                                                if ((unit[attacked_unit].player == PLAYER1) || (unit[attacked_unit].player == PLAYER3))
                                                {
                                                    nb_units_team1--;
                                                }
                                                else if ((unit[attacked_unit].player == PLAYER2) || (unit[attacked_unit].player == PLAYER4))
                                                {
                                                    nb_units_team2--;
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            ///si aucune unité n'est sélectionnée, vérifie si une unité est présente sur la case cliquée
                            if (!unit_is_selected) {

                                for (i=1; i<=nb_units; i++)
                                {
                                    if ((unit[i].position.x == tile.x) && (unit[i].position.y == tile.y) && ((current_player == unit[i].player) || (current_player == unit[i].player + 2)))
                                    {
                                    unit_is_selected = true;
                                    selected_unit = i;
                                    }
                                }
                            }
                            ///si une unité est sélectionnée et que l'on clique dessus, la déselectionner
                            else if (unit_is_selected && (tile.x == unit[selected_unit].position.x) && (tile.y == unit[selected_unit].position.y))
                            {

                                unit_is_selected = false;
                            }
                            ///sinon si une unité est sélectionnée et que l'on clique quelque part envisager un déplacement
                            else if (unit_is_selected)
                            {
                                int is_tile_occupied = false;

                                ///si la case sur laquelle on veut se déplacer est déjà occupée
                                for (i=1; i<=nb_units; i++)
                                {
                                    if ((tile.x == unit[i].position.x) && (tile.y == unit[i].position.y))
                                    {
                                        is_tile_occupied = true;
                                    }
                                }

                                ///déplacement unité si la case cliquée n'est pas occupée
                                if (!is_tile_occupied)
                                {
                                    int distance;
                                    int unit_move = false;

                                    ///calcul de la distance la plus courte séparant l'unité de sa déstination
                                    for (distance = 1; distance <= unit[selected_unit].movement && !unit_move; distance++)
                                    {
                                        if (can_reach_tile (unit[selected_unit].position, tile, distance, tile_max))
                                        {
                                            unit_move = true;

                                            ///déplacement
                                            unit[selected_unit].position.x = tile.x;
                                            unit[selected_unit].position.y = tile.y;

                                            ///diminution de la portée de déplcament
                                            unit[selected_unit].movement -= distance;
                                        }
                                    }
                                }
                            }

                        }
                    }
                    /*clique droit*/
                    if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        mouserightbutton_ispressed = true;
                    }

                }
                if (event.type == SDL_MOUSEMOTION)
                {
                    if (mouserightbutton_ispressed)
                    {
                        p.x = p.x + event.motion.xrel;
                        p.y = p.y + event.motion.yrel;
                    }
                }

                ///si fin du tour
                if (click_end_turn)
                {
                    unit_is_selected = false;
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderPresent(renderer);
                    click_end_turn = false;

                    ///les unités récupèrent leur capacité de mouvement et d'attaque
                    for (i=1; i<=nb_units; i++)
                    {
                        unit[i].movement = unit[i].stats.movement_base;
                        unit[i].can_attack = true;
                        unit[i].can_retaliate = true;
                    }

                    ///si toutes les unités d'une équipes ont été détruites => fin du jeu
                    if ((nb_units_team1 <= 0) || (nb_units_team2 <= 0))
                    {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        SDL_RenderClear(renderer);
                        POINT victorytext = {600, 200};
                        POINT factiontext = {600, 400};
                        display_text(renderer, "Victoire de :", "arial.ttf", 64, 255, 255, 255, victorytext);
                        switch (level.nb_players)
                        {
                            case 2:
                                if (nb_units_team2 <= 0) display_text(renderer, level.faction_player[1], "arial.ttf", 64, 255, 255, 255, factiontext);
                                else if (nb_units_team1 <= 0) display_text(renderer, level.faction_player[2], "arial.ttf", 64, 255, 255, 255, factiontext);
                                break;

                            case 3:
                                if (nb_units_team2 <= 0)
                                {
                                    display_text(renderer, level.faction_player[1], "arial.ttf", 64, 255, 255, 255, factiontext);
                                    factiontext.y += 100;
                                    display_text(renderer, level.faction_player[3], "arial.ttf", 64, 255, 255, 255, factiontext);
                                }
                                else if (nb_units_team1 <= 0) display_text(renderer, level.faction_player[2], "arial.ttf", 64, 255, 255, 255, factiontext);
                                break;

                            case 4:
                                if (nb_units_team2 <= 0)
                                {
                                    display_text(renderer, level.faction_player[1], "arial.ttf", 64, 255, 255, 255, factiontext);
                                    factiontext.y += 100;
                                    display_text(renderer, level.faction_player[3], "arial.ttf", 64, 255, 255, 255, factiontext);
                                }
                                else if (nb_units_team1 <= 0)
                                {
                                    display_text(renderer, level.faction_player[2], "arial.ttf", 64, 255, 255, 255, factiontext);
                                    factiontext.y += 100;
                                    display_text(renderer, level.faction_player[4], "arial.ttf", 64, 255, 255, 255, factiontext);
                                }
                                break;

                        }
                        SDL_RenderPresent(renderer);
                        wait(&endofgame);
                        game_isrunning = false;
                        SDL_RenderClear(renderer);

                    }
                    else if (current_player == PLAYER1)
                    {
                        current_player = PLAYER2;
                        display_image(renderer, Faction[2], center);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(1000);
                    }
                    else if (current_player == PLAYER2)
                    {

                        if (level.nb_players > 2)
                        {
                            current_player = PLAYER3;
                            display_image(renderer, Faction[3], center);

                        } else {
                            current_player = PLAYER1;
                            display_image(renderer, Faction[1], center);
                        }
                        SDL_RenderPresent(renderer);
                        SDL_Delay(1000);
                    }
                    else if (current_player == PLAYER3)
                    {
                        if (level.nb_players > 3)
                        {
                            current_player = PLAYER4;
                            display_image(renderer, Faction[4], center);

                        } else {
                            current_player = PLAYER1;
                            display_image(renderer, Faction[1], center);
                        }
                        SDL_RenderPresent(renderer);
                        SDL_Delay(1000);
                    }
                    else if (current_player == PLAYER4)
                    {
                        current_player = PLAYER1;
                        display_image(renderer, Faction[1], center);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(1000);
                    }
                    SDL_RenderPresent(renderer);
                }
            }

            /*** AFFICHAGE ***/

            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            display_image(renderer, background, center);

            ///affichage de la grille
            if (zoom == true)
            {
                display_image(renderer, gridzoom, p);
                display_image(renderer, endturn, button_end_turn);

            }
            else
            {
                display_image(renderer, grid, p);
                display_image(renderer, endturn, button_end_turn);
            }


            for (i=1; i<=nb_units; i++)
            {
                ///élimination des unités détruites
                if (unit[i].life <= 0)
                {
                    unit[i].position.x = 0;
                    unit[i].position.y = 0;
                }

                ///affichage de toutes les unités présentes sur le terrain
                if ((unit[i].position.x!=0) && (unit[i].position.y!=0))
                {
                    POINT unit_position = get_tile_center(grid, unit[i].position, p, gridzoom, zoom);
                    if (zoom) display_image(renderer, unit[i].imagezoom, unit_position);
                    else display_image(renderer, unit[i].image, unit_position);

                    ///affichage de la barre de vie
                    if (unit[i].life != unit[i].stats.life_base)
                    {
                        POINT lifebarlocation = {unit_position.x, unit_position.y - 25};
                        if (zoom)
                        {
                            lifebarlocation.y -= 40;
                        }
                        POINT damagebarlocation = {lifebarlocation.x + 45, lifebarlocation.y};
                        display_image(renderer, lifebar, lifebarlocation);
                        int damage = 10 - (10 * unit[i].life)/unit[i].stats.life_base;
                        int d;
                        for (d = 1; d <= damage; d++)
                        {
                            display_image(renderer, damagebar, damagebarlocation);
                            damagebarlocation.x -= 10;
                        }
                    }
                }
            }
            ///Si une unité est séléctionnée
            if (unit_is_selected)
            {
                ///affichage de la sélection et de la portée de mouvement
                selected = get_tile_center(grid, unit[selected_unit].position, p, gridzoom, zoom);
                TILE movement_tile;
                for (movement_tile.x = unit[selected_unit].position.x - unit[selected_unit].movement; movement_tile.x <= unit[selected_unit].position.x + unit[selected_unit].movement; movement_tile.x++)
                {
                    for(movement_tile.y = unit[selected_unit].position.y - unit[selected_unit].movement; movement_tile.y <= unit[selected_unit].position.y + unit[selected_unit].movement; movement_tile.y++)
                    {
                        if ((movement_tile.x != unit[selected_unit].position.x) || (movement_tile.y != unit[selected_unit].position.y))
                        {
                            if (can_reach_tile (unit[selected_unit].position, movement_tile, unit[selected_unit].movement, tile_max))
                            {
                                POINT movement_tile_center = get_tile_center(grid, movement_tile, p, gridzoom, zoom);
                                if (zoom) display_image(renderer, movementzoom, movement_tile_center);
                                else display_image(renderer, movement, movement_tile_center);
                            }
                        }
                    }
                }
                if (zoom) display_image(renderer, selectionzoom, selected);
                else display_image(renderer, selection, selected);


                ///affichage de la détection des unités ennemies
                for (i=1; i<=nb_units; i++)
                {
                    if ((unit[i].player != unit[selected_unit].player) && (unit[i].player != unit[selected_unit].player + 2))
                    {
                        if(can_reach_tile (unit[selected_unit].position, unit[i].position, unit[selected_unit].stats.range, tile_max))
                        {
                            POINT attack_tile_center = get_tile_center(grid, unit[i].position, p, gridzoom, zoom);
                            if (zoom) display_image(renderer, attackzoom, attack_tile_center);
                            else display_image(renderer, attack, attack_tile_center);
                        }
                    }
                }
                POINT p_unitscreen = {155, 105};
                display_image(renderer, unit_screen, p_unitscreen);
                display_image(renderer, unit[selected_unit].icon, p_unitscreen);
                POINT p_unitname = {310, 10};
                display_text(renderer, unit[selected_unit].name, "arial.ttf", 64, 255, 255, 255, p_unitname);
            }

            ///affichage du bouton zoom
            if (zoom == true) display_image(renderer, zoomout, button_zoom);
            else display_image(renderer, zoomin, button_zoom);

            SDL_RenderPresent(renderer);

            /*** FIN AFFICHAGE ***/
    }
    SDL_RenderClear(renderer);

    ///déchargement des textures
    unload_image(renderer, grid);
    unload_image(renderer, background);
    unload_image(renderer, gridzoom);
    unload_image(renderer, selection);
    unload_image(renderer, selectionzoom);
    unload_image(renderer, movement);
    unload_image(renderer, movementzoom);
    unload_image(renderer, attack);
    unload_image(renderer, attackzoom);
    unload_image(renderer, explosion);
    unload_image(renderer, damagebar);
    unload_image(renderer, lifebar);

    ///déchargement du Graphical User Interface
    unload_image(renderer, ingame_menu);
    unload_image(renderer, main_menu);
    unload_image(renderer, quit);
    unload_image(renderer, backtogame);
    unload_image(renderer, savegame);
    unload_image(renderer, zoomin);
    unload_image(renderer, zoominpressed);
    unload_image(renderer, zoomout);
    unload_image(renderer, zoomoutpressed);
    unload_image(renderer, endturn);
    unload_image(renderer, endturnpressed);
    unload_image(renderer, unit_screen);


    ///déchargement des images associées aux factions
    for (i = 1; i<=level.nb_players; i++)
    {
        unload_image(renderer, Faction[i]);
    }



    ///déchargement des images associées aux différentes unités
    for (i=0; i<nb_images; i++)
    {
        unload_image(renderer, unit_sprite[i]);
        unload_image(renderer, unit_sprite_zoom[i]);
        unload_image(renderer, unit_sprite_icon[i]);
    }

    ///déchargement des noms associés aux différentes unités
    for (i=1; i<=nb_units; i++)
    {
        free(unit[i].name);
    }

    ///déchargement des noms associés aux factions
    for (i = 1; i<=level.nb_players; i++)
    {
        free(level.faction_player[i]);
    }

}

///fonction d'initialisation des unités en début de partie
void init_game(UNIT unit[], SPRITE unit_sprite[], SPRITE unit_sprite_zoom[], SPRITE unit_sprite_icon[], LEVEL level)
{
    int i;

    for (i=1; i<level.nb_units; i++)
    {
        unit[i] = define_unit(unit[i], unit_sprite, unit_sprite_zoom, unit_sprite_icon, level.unit[i].name);
        unit[i].player = level.unit[i].player;
        unit[i].position.x = level.unit[i].position.x;
        unit[i].position.y = level.unit[i].position.y;
        free(level.unit[i].name);
        free(level.unit);
    }
}

///fonction d'attaque de 1 sur 2
///retourne l'unité attaquée
UNIT unit_attack(UNIT unit1, UNIT unit2, int zoom, SDL_Renderer *renderer, SPRITE grid, SPRITE gridzoom, POINT grid_center, SPRITE explosion, SPRITE explosionzoom)
{
    if (zoom) display_image(renderer, explosionzoom, get_tile_center(grid, unit2.position, grid_center, gridzoom, zoom));
    else display_image(renderer, explosion, get_tile_center(grid, unit2.position, grid_center, gridzoom, zoom));
    SDL_RenderPresent(renderer);
    SDL_Delay(200);
    ///Ajouter types
    unit2.life -= unit1.stats.attack_base;
    return unit2;
}
/**TO DO
- main menu button on level screen, more stuff on level screen
- hover on buttons
- finish main menu
- add backgrounds
- GUI Unit (name, HP, etc)
- Unit types, attack modifier, stats
- make units
- make save game/load game function
- add different maps/change map biomes
OPTIONAL : update system, version
OPTIONAL : controls menu
OPTIONAL : resolution detection
OPTIONAL : settings : language
OPTIONAL : add sound, music **/
