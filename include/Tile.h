#ifndef TILE_H
#define TILE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class Tile
{
public:
    Tile(SDL_Renderer *renderer, char letter, int points, const std::string &lettersPath, int index);
    Tile(const Tile &other); // Copy constructor
    ~Tile();
    void render(int x, int y);
    void setPosition(int x, int y);
    int getX() const;
    int getY() const;
    void setDragging(bool dragging);
    bool isDragging() const;

private:
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int x, y;
    bool dragging;
    char letter;
    int points;
};

#endif