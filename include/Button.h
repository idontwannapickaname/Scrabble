#ifndef BUTTON_H
#define BUTTON_H
#include <SDL2/SDL.h>
#include <string>

class Button
{
private:
    SDL_Renderer *renderer;
    int x, y, width, height;
    std::string text;
    SDL_Color color;
    SDL_Texture *texture;

public:
    Button(SDL_Renderer *renderer, int x, int y, int width, int height, const std::string &text, SDL_Color color);
    ~Button();
    void render();
    bool isClicked(int mouseX, int mouseY);
};

#endif