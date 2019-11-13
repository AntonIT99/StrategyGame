#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <dirent.h>
#include "main.h"
#include "game.h"
#include "display.h"
#include "locate.h"

UNIT define_unit(UNIT unit, SPRITE unit_sprite[], SPRITE unit_sprite_zoom[], SPRITE unit_sprite_icon[], char* unit_id)
{
    unit.id = unit_id;
    //strcpy(unit.id, unit_id);

    DIR* dir = NULL;
    struct dirent* file = NULL;
    dir = opendir("units");

    if (dir != NULL)
    {
        while((file = readdir(dir)) != NULL)
        {
            char* id = strtok(file->d_name, ".");

            if (id != NULL)
            {
                if (strcmp(unit.id , id) == 0)
                {
                    char path[100] = "units/";
                    strncat(path, id, 100);
                    strncat(path, ".txt", 100);
                    FILE* file = fopen(path, "r");

                    if (file != NULL)
                    {
                        char line[200];

                        while (fgets(line, 200, file) != NULL)
                        {
                            if (strstr(line, "name") != NULL)
                            {
                                char* name = strtok(line, ":");
                                name = strtok(NULL, ":");
                                name = strtok(name, "\n");
                                unit.name = malloc(200*sizeof(char));
                                strcpy(unit.name, name);
                            }
                            else if (strstr(line, "image") != NULL)
                            {
                                char* image = strtok(line, ":");
                                image = strtok(NULL, ":");
                                image = strtok(image, "\n");

                                DIR* assets = NULL;
                                struct dirent* asset_file = NULL;
                                int image_id = 0;

                                ///retrouve à partir du nom de l'image, sa position dans l'ordre alphabétique

                                dir = opendir("assets/units");
                                while(((asset_file = readdir(dir)) != NULL) && (strstr(asset_file->d_name, image) == NULL))
                                {
                                    if (strstr(asset_file->d_name, ".png") != NULL)
                                    {
                                        image_id++;
                                    }
                                }
                                closedir(assets);

                                unit.image = unit_sprite[image_id];
                                unit.imagezoom = unit_sprite_zoom[image_id];
                                unit.icon = unit_sprite_icon[image_id];

                            }
                            else if (strstr(line, "life") != NULL)
                            {
                                char* life = strtok(line, ":");
                                life = strtok(NULL, ":");
                                life = strtok(life, "\n");
                                unit.life = atoi(life);
                                unit.stats.life_base = atoi(life);
                            }
                            else if (strstr(line, "attack_base") != NULL)
                            {
                                char* attack = strtok(line, ":");
                                attack = strtok(NULL, ":");
                                attack = strtok(attack, "\n");
                                unit.stats.attack_base = atoi(attack);
                            }
                            else if (strstr(line, "attack_infantry") != NULL)
                            {
                                char* attack = strtok(line, ":");
                                attack = strtok(NULL, ":");
                                attack = strtok(attack, "\n");
                                unit.stats.attack_infantry = atoi(attack);
                            }
                            else if (strstr(line, "attack_vehicle") != NULL)
                            {
                                char* attack = strtok(line, ":");
                                attack = strtok(NULL, ":");
                                attack = strtok(attack, "\n");
                                unit.stats.attack_vehicle = atoi(attack);
                            }
                            else if (strstr(line, "attack_plane") != NULL)
                            {
                                char* attack = strtok(line, ":");
                                attack = strtok(NULL, ":");
                                attack = strtok(attack, "\n");
                                unit.stats.attack_plane = atoi(attack);
                            }
                            else if (strstr(line, "resistance") != NULL)
                            {
                                char* resistance = strtok(line, ":");
                                resistance = strtok(NULL, ":");
                                resistance = strtok(resistance, "\n");
                                unit.stats.resistance = atoi(resistance);
                            }
                            else if (strstr(line, "range") != NULL)
                            {
                                char* range = strtok(line, ":");
                                range = strtok(NULL, ":");
                                range = strtok(range, "\n");
                                unit.stats.range = atoi(range);
                            }
                            else if (strstr(line, "movement") != NULL)
                            {
                                char* movement = strtok(line, ":");
                                movement = strtok(NULL, ":");
                                movement = strtok(movement, "\n");
                                unit.movement = atoi(movement);
                                unit.stats.movement_base = atoi(movement);
                            }
                        }

                    } else {

                        printf("File %s.txt could not be found\n", id);
                    }
                    fclose(file);
                }
            }
        }

    } else {

        printf("The directory units could not be found\n");
    }
    closedir(dir);
    return unit;
}
