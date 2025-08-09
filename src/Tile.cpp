#include "../include/Tile.h"
#include <iostream>

Tile::Tile(SDL_Renderer *renderer, char letter, int points, const std::string &lettersPath, int index)
    : renderer(renderer), x(0), y(0), dragging(false), letter(letter), points(points)
{
    SDL_Surface *surface = IMG_Load(lettersPath.c_str());
    if (!surface)
    {
        std::cerr << "Unable to load letters image! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Assuming letters are horizontal, 64x64 each, A at 0, B at 64, etc.
    // But adjust if your image layout differs. Image shows A-Z in one row.
}

Tile::Tile(const Tile &other)
    : renderer(other.renderer), x(other.x), y(other.y), dragging(other.dragging), letter(other.letter), points(other.points)
{
    // Share texture or copy if needed, but for simplicity assume shared texture
    texture = other.texture; // Warning: This assumes texture is shared; manage lifetime accordingly
}

Tile::~Tile()
{
    // Do not destroy texture here if shared
}

void Tile::render(int x, int y)
{
    int index = letter - 'A';               // Assuming A=0, B=1, etc.
    SDL_Rect src = {index * 64, 0, 64, 64}; // Adjust if Y has different row
    if (letter == 'Y')
        index = 24; // Adjust based on image
    if (letter == 'Z')
        index = 25;
    SDL_Rect dst = {x, y, 64, 64};
    SDL_RenderCopy(renderer, texture, &src, &dst);
}

void Tile::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

int Tile::getX() const { return x; }
int Tile::getY() const { return y; }

void Tile::setDragging(bool dragging) { this->dragging = dragging; }
bool Tile::isDragging() const { return dragging; }
