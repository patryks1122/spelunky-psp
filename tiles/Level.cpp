//
// Created by xdbeef on 04.03.18.
//

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <GL/gl.h>
#include "Level.hpp"
#include "SplashScreenType.hpp"
#include "../rooms/EntranceRooms.hpp"
#include "../rooms/ExitRooms.hpp"
#include "../rooms/ClosedRooms.hpp"
#include "../rooms/LeftRightRooms.hpp"
#include "../rooms/LeftRightDownRooms.hpp"
#include "../rooms/LeftRightUpRooms.hpp"
#include "../rooms/SplashScreenRooms.hpp"
#include "../rooms/AltarRoom.hpp"
#include "../rooms/RoomType.hpp"
#include "LevelRenderingUtils.hpp"
#include "../rooms/ShopRooms.hpp"
#include "Direction.hpp"

#define SYS 1
#define LOGME 0

#include "../glchk.h"

/**
 * Copies clean cave background to the map, then writes current tiles from map_tiles[32][32] to the map,
 * formats it in a way that makes it understandable by the graphics engines and copies it to the place
 * the engines expect the map to be.
 * One should call this function, when he knows that the map changed,
 * i.e a bomb exploded or he moved from main menu to game level.
 */
void Level::update_level() {
    write_tiles_to_map(); //now we write our tiles onto the current map
}

void Level::generate_frame() {

    //upper row
    for (int a = 0; a < MAP_GAME_HEIGHT_TILES; a++) {
//        delete (map_tiles[a][0]);
//        auto *t = new MapTile();
        map_tiles[a][0]->match_tile(CAVE_REGULAR);
        map_tiles[a][0]->destroyable = false; //make it non-destroyable explicitly
        map_tiles[a][0]->x = a;
        map_tiles[a][0]->y = 0;
        map_tiles[a][0]->exists = true;
    }

    //bottom row
    for (int a = 0; a < MAP_GAME_WIDTH_TILES; a++) {
//        delete (map_tiles[31][a]);
//        auto *t = new MapTile();
        map_tiles[31][a]->match_tile(CAVE_REGULAR);
        map_tiles[31][a]->destroyable = false; //make it non-destroyable explicitly
        map_tiles[31][a]->x = 31;
        map_tiles[31][a]->y = a;
        map_tiles[31][a]->exists = true;
    }

    //right row
    for (int a = 0; a < MAP_GAME_HEIGHT_TILES; a++) {
//        delete (map_tiles[31][a]);
//        auto *t = new MapTile();
        map_tiles[a][31]->match_tile(CAVE_REGULAR);
        map_tiles[a][31]->destroyable = false; //make it non-destroyable explicitly
        map_tiles[a][31]->x = a;
        map_tiles[a][31]->y = 31;
        map_tiles[a][31]->exists = true;
    }

    //left row
    for (int a = 0; a < MAP_GAME_WIDTH_TILES; a++) {
//        delete (map_tiles[0][a]);
//        auto *t = new MapTile();
        map_tiles[0][a]->match_tile(CAVE_REGULAR);
        map_tiles[0][a]->destroyable = false; //make it non-destroyable explicitly
        map_tiles[0][a]->x = 0;
        map_tiles[0][a]->y = a;
        map_tiles[0][a]->exists = true;
    }
}

/**
 * Writes bytes that make a graphical representation of every MapTile in map_tiles array to the current map.
 */
void Level::write_tiles_to_map() {

    // In opengl:
    // The top-left corner will be at (-1, 1).
    // In spelunkyds:
    // top-left corner is 0,0 and right-lower is 1, 1

    // iterating from left-lower corner of the room to the right-upper (spelunky-ds convention)
    for (int x = 0; x < MAP_GAME_WIDTH_TILES; x++) {
        for (int y = 0; y < MAP_GAME_HEIGHT_TILES; y++) {
            MapTile *t = map_tiles[x][y];
//            if (t->exists) {
                int tile_type = t->mapTileType;
                if (tile_type > 0) {
                    upload_tile(tile_type);
                } else {
                    // place a piece of background
                    // FIXME Find correct order of placing background tiles and put it into this matrix
                    int bg_matrix[8][2] = {
                            {15, 16},
                            {13, 14},
                            {11, 12},
                            {9,  10},
                            {7,  8},
                            {5,  6},
                            {3,  4},
                            {1,  2},
                    };

                    int bgr_type = bg_matrix[y % 8][x % 2] + 42;
                    upload_tile(bgr_type);
                }

                GLCHK(glMatrixMode(GL_MODELVIEW));
                GLCHK(glLoadIdentity());
                GLCHK(glTranslatef(camera->x + x, camera->y + y, 0));
                dumpmat(GL_MODELVIEW_MATRIX, "trans modelview");
                //GLCHK(glRotatef(angle * 1.32f, 0.0f, 0.0f, 1.0f));
                //dumpmat(GL_MODELVIEW_MATRIX, "rot modelview");
                //FIXME
                GLCHK(glBindTexture(GL_TEXTURE_2D, 0));

                glBegin(GL_TRIANGLE_FAN);
                glColor3f(1, 0, 0);
                glTexCoord2f(0, 0);
                glVertex3f(0, 0, 0);

                glColor3f(0, 1, 0);
                glTexCoord2f(0, 1);
                glVertex3f(0, 1, 0);

                glColor3f(0, 0, 1);
                glTexCoord2f(1, 1);
                glVertex3f(1, 1, 0);

                glColor3f(1, 1, 1);
                glTexCoord2f(1, 0);
                glVertex3f(1, 0, 0);
                GLCHK(glEnd());
//            }
        }
    }
}


/*
   F E D C B A 9 8|7 6 5 4 3 2 1 0
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |R R R R R|G G G G G G|B B B B B| 565
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |R R R R R|G G G G G|B B B B B|A| 5551
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |R R R R|G G G G|B B B B|A A A A| 4444
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

extern unsigned char gfxcavebg_start[];
#define RGBA4444(r, g, b, a)    ((((r) << 8) & 0xf000) | (((g) << 4) & 0x0f00) | (((b)     ) & 0x00f0) | (((a) >> 4) & 0x000f))

void Level::upload_tile(int type) {

    int current_tile;

    if (type <= 0)
        current_tile = static_cast<unsigned int>(ALTAR_LEFT) - 1;
    else
        current_tile = type - 1;

    unsigned int row = static_cast<unsigned int>(ceil((current_tile / 2) + 1));
    int offset = (row - 1) * 2 * 16 * 16;

    if (offset < 0)
        exit(0);

    bool row_offset = (current_tile % 2 == 1);

    unsigned int index = 0;
    if (row_offset) index += 16;

    bool first = false;

    unsigned short tex16[16 * 16];

    for (int i = 0; i < 16 * 16; i++) {

        if (i % 16 == 0 && first) {
            index += 16;
        }

        tex16[i] = RGBA4444(gfxcavebg_start[((offset + index) * 4) + 0],
                            gfxcavebg_start[((offset + index) * 4) + 1],
                            gfxcavebg_start[((offset + index) * 4) + 2],
                            gfxcavebg_start[((offset + index) * 4) + 3]);

        index++;
        first = true;
    }
//FIXME
//    GLCHK(glBindTexture(GL_TEXTURE_2D, texture_indexes[0]));

    GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
    GLCHK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_SHORT_4_4_4_4, tex16));
    GLCHK(glEnable(GL_TEXTURE_2D));

}


/**
 * Initialises every MapTile in the map_tiles[][] array with its position on the map,
 * so it could be allowed to call LevelGenerator::tiles_to_map.
 * It utilizes given splash screen type, to know if any tile should be planted on the place it iterates,
 * and what MapTileType it should have.
 */
void Level::initialise_tiles_from_splash_screen(SplashScreenType splash_type) {

    int tab[SPLASH_SCREEN_HEIGHT][SPLASH_SCREEN_WIDTH];
    bool offset_on_upper_screen = false;

    if (splash_type == ON_LEVEL_DONE_UPPER || splash_type == SCORES_UPPER || splash_type == MAIN_MENU_UPPER) {
        offset_on_upper_screen = true;

        if (splash_type == MAIN_MENU_UPPER)
            memcpy(tab, main_menu_upper, sizeof(main_menu_upper));
        else if (splash_type == ON_LEVEL_DONE_UPPER)
            memcpy(tab, on_level_done_upper, sizeof(on_level_done_upper));
        else if (splash_type == SCORES_UPPER)
            memcpy(tab, scores_upper, sizeof(scores_upper));
    }

    if (splash_type == ON_LEVEL_DONE_LOWER || splash_type == SCORES_LOWER || splash_type == MAIN_MENU_LOWER) {

        if (splash_type == ON_LEVEL_DONE_LOWER)
            memcpy(tab, on_level_done_lower, sizeof(on_level_done_lower));
        else if (splash_type == MAIN_MENU_LOWER)
            memcpy(tab, main_menu_lower, sizeof(main_menu_lower));
        else if (splash_type == SCORES_LOWER)
            memcpy(tab, scores_lower, sizeof(scores_lower));
    }

    //Now we initialise every tile in the splash screen and give it a map_index, which describes its location
    for (int tab_y = 0; tab_y < SPLASH_SCREEN_HEIGHT; tab_y++) {
        for (int tab_x = 0; tab_x < SPLASH_SCREEN_WIDTH; tab_x++) {

            if (tab[tab_y][tab_x] != 0) {

                //offset to the position in current room
                int room_offset =
                        static_cast<int>(
                                2 * ROOM_TILE_HEIGHT_SPLASH_SCREEN *
                                LINE_WIDTH * ((ROOMS_Y - offset_on_upper_screen) - 1) - 4 * OFFSET_Y);
                //pos x and y in pixels of the tile in the current room
                int pos_x = static_cast<int>((tab_x * 2) / 2);
                //NDS engine has different coordinate system than our room layout map,
                //so we invert the Y axis by ((ROOMS_Y - offset_on_upper_screen) - 1)
                int pos_y = static_cast<int>(
                        tab_y + ROOM_TILE_HEIGHT_SPLASH_SCREEN * ((ROOMS_Y - offset_on_upper_screen) - 1) - 4);

                map_tiles[pos_x][pos_y]->match_tile(static_cast<MapTileType>(tab[tab_y][tab_x]));
                room_offset + (tab_x * 2) + (LINE_WIDTH + (tab_y * LINE_WIDTH * 2)) + 1;
                map_tiles[pos_x][pos_y]->x = pos_x;
                map_tiles[pos_x][pos_y]->y = pos_y;
                map_tiles[pos_x][pos_y]->exists = true;

            }
        }
    }
}

/**
 * Initialises every MapTile in the map_tiles[][] array with its position on the map,
 * so it could be allowed to call LevelGenerator::tiles_to_map.
 * It utilizes current room layout, to know if any tile should be planted on the place it iterates,
 * and what MapTileType it should have.
 */
void Level::initialise_tiles_from_room_layout() {

    //a room, 10x10 tiles
    int tab[10][10];
    int r;

    //iterate through every room we have
    for (int room_y = ROOMS_Y - 1; room_y >= 0; room_y--) {
        for (int room_x = 0; room_x < ROOMS_X; room_x++) {

            //basing on the room type, randomly select a variation of this room
            //and copy it to the temporary tab[10][10] array
            int room_type = layout[room_x][room_y];
            r = rand() % 6;
            layout_room_ids[room_x][room_y] = r; //-1 if completely disabling NPC's in this room

            //copying specific room variation
            switch (room_type) {
                case R_CLOSED:
                    memcpy(tab, closed_rooms[r], sizeof(closed_rooms[r]));
                    break;
                case R_LEFT_RIGHT:
                    memcpy(tab, left_right_rooms[r], sizeof(left_right_rooms[r]));
                    break;
                case R_LEFT_RIGHT_DOWN:
                    memcpy(tab, left_right_down_rooms[r], sizeof(left_right_down_rooms[r]));
                    break;
                case R_LEFT_RIGHT_UP:
                    memcpy(tab, left_right_up_rooms[r], sizeof(left_right_up_rooms[r]));
                    break;
                case R_ENTRANCE:
                    memcpy(tab, entrance_room[r], sizeof(entrance_room[r]));
                    break;
                case R_EXIT:
                    memcpy(tab, exit_room[r], sizeof(exit_room[r]));
                    break;
                case R_SHOP_LEFT:
                    memcpy(tab, shops[0], sizeof(shops[0]));
                    break;
                case R_SHOP_RIGHT:
                    memcpy(tab, shops[1], sizeof(shops[1]));
                    break;
                case R_SHOP_LEFT_MUGSHOT:
                    memcpy(tab, shops_mugshots[0], sizeof(shops_mugshots[0]));
                    break;
                case R_SHOP_RIGHT_MUGSHOT:
                    memcpy(tab, shops_mugshots[1], sizeof(shops_mugshots[1]));
                    break;
                case R_ALTAR:
                    memcpy(tab, altar_room[0], sizeof(altar_room[1]));
                    break;
                default:
                    break;
            }

            //Now we initialise every tile in map and give it a map_index, which describes its location
            for (int tab_y = 0; tab_y < ROOM_TILE_HEIGHT_GAME; tab_y++) {
                for (int tab_x = 0; tab_x < ROOM_TILE_WIDTH_GAME; tab_x++) {

                    if (tab[tab_y][tab_x] != 0) {

                        //pos x and y in pixels of the tile in the current room
                        int pos_x = static_cast<int>((OFFSET_X + tab_x * 2 + 2 * ROOM_TILE_WIDTH_GAME * room_x) / 2);
                        //NDS engine has different coordinate system than our room layout map,
                        //so we invert the Y axis by ((ROOMS_Y - room_y) - 1))
                        int pos_y = static_cast<int>(
                                (OFFSET_X + tab_y * 2 + 2 * ROOM_TILE_HEIGHT_GAME * ((ROOMS_Y - room_y) - 1)) / 2);

                        map_tiles[pos_x][pos_y]->match_tile(static_cast<MapTileType>(tab[tab_y][tab_x]));

                        map_tiles[pos_x][pos_y]->x = pos_x;
                        map_tiles[pos_x][pos_y]->y = pos_y;
                        map_tiles[pos_x][pos_y]->exists = true;

                    }
                }
            }
        }
    }
}

/**
 * Sets given MapTile to the first tile on map that matches given MapTileType.
 * Searching begins on the upper-left corner of the map and goes right-down.
 * https://stackoverflow.com/questions/416162/assignment-inside-function-that-is-passed-as-pointer
 * @param mapTileType
 * @param m
 */
void Level::get_first_tile_of_given_type(MapTileType mapTileType, MapTile *&m) {
    for (int a = 0; a < MAP_GAME_WIDTH_TILES; a++) {
        for (int b = 0; b < MAP_GAME_HEIGHT_TILES; b++) {
            if (map_tiles[a][b]->exists && map_tiles[a][b]->mapTileType == mapTileType) {
                m = map_tiles[a][b];
                break;
            }
        }
    }
}

void Level::init_map_tiles() {
    for (int a = 0; a < 32; a++)
        for (int b = 0; b < 32; b++)
            map_tiles[a][b] = new MapTile();
}

void Level::clean_map_layout() {
    //clean current layout
    for (int x = 0; x < 32; x++)
        for (int y = 0; y < 32; y++) {
            map_tiles[x][y]->exists = false;
            map_tiles[x][y]->destroyable = true; //tiles are destroyable by default
        }
}
