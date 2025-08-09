#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include "../include/Board.h"
#include "../include/Tile.h"
#include "../include/PlayerRack.h"

const int SCREEN_WIDTH = 960;   // 15*64 = 960
const int SCREEN_HEIGHT = 1024; // Board 960 + rack area

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Scrabble Board", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    Board board(renderer, "assets/images/TDTile-Black-64x64.png");             // Assuming the tileset image is this
    PlayerRack rack(renderer, "assets/images/Thick-Wood_01a-Black-64x64.png"); // Replace with your letters image filename

    // Sample tiles for rack (A, B, C for demo)
    rack.addTile(Tile(renderer, 'A', 1, "assets/images/Thick-Wood_01a-Black-64x64.png", 0)); // Assuming letters.png has letters in order, index 0 for A
    rack.addTile(Tile(renderer, 'B', 3, "assets/images/Thick-Wood_01a-Black-64x64.png", 1));
    rack.addTile(Tile(renderer, 'C', 3, "assets/images/Thick-Wood_01a-Black-64x64.png", 2));

    Tile *draggedTile = nullptr;
    int dragOffsetX = 0, dragOffsetY = 0;

    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                draggedTile = rack.getTileAt(x, y - 960); // Rack is below board at y=960
                if (draggedTile)
                {
                    dragOffsetX = x - draggedTile->getX();
                    dragOffsetY = y - draggedTile->getY();
                    draggedTile->setDragging(true);
                }
            }
            else if (e.type == SDL_MOUSEMOTION && draggedTile)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                draggedTile->setPosition(x - dragOffsetX, y - dragOffsetY);
            }
            else if (e.type == SDL_MOUSEBUTTONUP && draggedTile)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                // Snap to board if dropped on board
                if (y < 960)
                {
                    int boardX = x / 64;
                    int boardY = y / 64;
                    if (boardX >= 0 && boardX < 15 && boardY >= 0 && boardY < 15)
                    {
                        board.placeTile(boardX, boardY, *draggedTile);
                        rack.removeTile(draggedTile);
                    }
                }
                draggedTile->setDragging(false);
                draggedTile = nullptr;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        board.render(0, 0);
        rack.render(0, 960); // Render rack below the board

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
