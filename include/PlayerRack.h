#ifndef PLAYERRACK_H
#define PLAYERRACK_H

#include <SDL2/SDL.h>
#include <vector>
#include "Tile.h"

class PlayerRack
{
public:
    PlayerRack(SDL_Renderer *renderer, const std::string &lettersPath);
    ~PlayerRack();
    void addTile(const Tile &tile);
    void removeTile(Tile *tile);
    Tile *getTileAt(int x, int y); // Relative to rack position
    void render(int x, int y);

private:
    SDL_Renderer *renderer;
    std::vector<Tile> tiles;
    std::string lettersPath;
    void arrangeTiles();
};

#endif