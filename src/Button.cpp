#include "../include/Button.h"
#include <SDL2/SDL_ttf.h>
#include <iostream>

Button::Button(SDL_Renderer *renderer, int x, int y, int width, int height, const std::string &text, SDL_Color color)
    : renderer(renderer), x(x), y(y), width(width), height(height), text(text), color(color), texture(nullptr)
{
    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return;
    }

    TTF_Font *font = TTF_OpenFont("assets/fonts/FreeSans.ttf", 24);
    if (!font)
    {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255, 255});
    if (!surface)
    {
        std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
    }
    else
    {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    TTF_CloseFont(font);
}

Button::~Button()
{
    SDL_DestroyTexture(texture);
}

void Button::render()
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderFillRect(renderer, &rect);

    if (texture)
    {
        int texW, texH;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
        SDL_Rect dst = {x + (width - texW) / 2, y + (height - texH) / 2, texW, texH};
        SDL_RenderCopy(renderer, texture, nullptr, &dst);
    }
}

bool Button::isClicked(int mouseX, int mouseY)
{
    return mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height;
}