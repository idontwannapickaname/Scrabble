#include "../include/PlayerRack.h"

PlayerRack::PlayerRack(SDL_Renderer *renderer, const std::string &lettersPath)
    : renderer(renderer), lettersPath(lettersPath) {}

PlayerRack::~PlayerRack() {}

void PlayerRack::addTile(const Tile &tile)
{
    tiles.push_back(tile);
    arrangeTiles();
}

void PlayerRack::removeTile(Tile *tile)
{
    for (auto it = tiles.begin(); it != tiles.end(); ++it)
    {
        if (&(*it) == tile)
        {
            tiles.erase(it);
            break;
        }
    }
    arrangeTiles();
}

Tile *PlayerRack::getTileAt(int x, int y)
{
    for (auto &tile : tiles)
    {
        if (x >= tile.getX() && x < tile.getX() + 64 &&
            y >= tile.getY() && y < tile.getY() + 64 &&
            !tile.isDragging())
        {
            return &tile;
        }
    }
    return nullptr;
}

void PlayerRack::render(int x, int y)
{
    for (auto &tile : tiles)
    {
        if (!tile.isDragging())
        {
            tile.render(x + tile.getX(), y + tile.getY());
        }
        else
        {
            tile.render(tile.getX(), tile.getY()); // Absolute when dragging
        }
    }
}

void PlayerRack::arrangeTiles()
{
    int offset = 0;
    for (auto &tile : tiles)
    {
        tile.setPosition(offset * 64, 0);
        offset++;
    }
}
