#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "Tile.h"

enum TileType
{
    NORMAL, // 0: blank
    DL,     // 1: Double Letter
    DW,     // 2: Double Word
    TL,     // 3: Triple Letter
    TW,     // 4: Triple Word
    CENTER  // 5: Star (center, acts as DW)
};

class Board
{
public:
    Board(SDL_Renderer *renderer, const std::string &tilesetPath);
    ~Board();
    void render(int x, int y);
    void placeTile(int boardX, int boardY, const Tile &tile);

private:
    SDL_Renderer *renderer;
    SDL_Texture *tilesetTexture;
    std::vector<std::vector<TileType>> boardLayout; // 15x15 grid types
    std::vector<std::vector<Tile *>> placedTiles;   // Placed tiles on board
    void initBoardLayout();
    SDL_Rect getTileSrcRect(TileType type);
};

#endif