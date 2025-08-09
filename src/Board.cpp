#include "../include/Board.h"
#include <iostream>

Board::Board(SDL_Renderer *renderer, const std::string &tilesetPath) : renderer(renderer)
{
    SDL_Surface *surface = IMG_Load(tilesetPath.c_str());
    if (!surface)
    {
        std::cerr << "Unable to load tileset! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    tilesetTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    boardLayout.resize(15, std::vector<TileType>(15, NORMAL));
    placedTiles.resize(15, std::vector<Tile *>(15, nullptr));
    initBoardLayout();
}

Board::~Board()
{
    SDL_DestroyTexture(tilesetTexture);
    for (auto &row : placedTiles)
    {
        for (auto &tile : row)
        {
            delete tile;
        }
    }
}

void Board::initBoardLayout()
{
    // Triple Word Scores (red)
    std::vector<std::pair<int, int>> twPositions = {
        {0, 0}, {0, 7}, {0, 14}, {7, 0}, {7, 14}, {14, 0}, {14, 7}, {14, 14}};
    for (auto &pos : twPositions)
    {
        boardLayout[pos.first][pos.second] = TW;
    }

    // Double Word Scores (pink), including center
    std::vector<std::pair<int, int>> dwPositions = {
        {1, 1}, {2, 2}, {3, 3}, {4, 4}, {1, 13}, {2, 12}, {3, 11}, {4, 10}, {10, 4}, {11, 3}, {12, 2}, {13, 1}, {10, 10}, {11, 11}, {12, 12}, {13, 13}, {7, 7} // Center
    };
    for (auto &pos : dwPositions)
    {
        boardLayout[pos.first][pos.second] = (pos.first == 7 && pos.second == 7) ? CENTER : DW;
    }

    // Triple Letter Scores (dark blue)
    std::vector<std::pair<int, int>> tlPositions = {
        {0, 3}, {0, 11}, {2, 6}, {2, 8}, {3, 0}, {3, 7}, {3, 14}, {6, 2}, {6, 6}, {6, 8}, {6, 12}, {7, 3}, {7, 11}, {8, 2}, {8, 6}, {8, 8}, {8, 12}, {11, 0}, {11, 7}, {11, 14}, {12, 6}, {12, 8}, {14, 3}, {14, 11}};
    for (auto &pos : tlPositions)
    {
        boardLayout[pos.first][pos.second] = TL;
    }

    // Double Letter Scores (light blue)
    std::vector<std::pair<int, int>> dlPositions = {
        {0, 5}, {0, 9}, {1, 2}, {1, 6}, {1, 8}, {1, 12}, {2, 0}, {2, 3}, {2, 7}, {2, 11}, {2, 14}, {3, 1}, {3, 4}, {3, 10}, {3, 13}, {4, 2}, {4, 5}, {4, 9}, {4, 12}, {5, 0}, {5, 3}, {5, 6}, {5, 8}, {5, 11}, {5, 14}, {6, 1}, {6, 4}, {6, 7}, {6, 10}, {6, 13}, {7, 2}, {7, 5}, {7, 9}, {7, 12}, {8, 1}, {8, 4}, {8, 7}, {8, 10}, {8, 13}, {9, 0}, {9, 3}, {9, 6}, {9, 8}, {9, 11}, {9, 14}, {10, 2}, {10, 5}, {10, 9}, {10, 12}, {11, 1}, {11, 4}, {11, 10}, {11, 13}, {12, 0}, {12, 3}, {12, 7}, {12, 11}, {12, 14}, {13, 2}, {13, 6}, {13, 8}, {13, 12}, {14, 5}, {14, 9}};
    for (auto &pos : dlPositions)
    {
        boardLayout[pos.first][pos.second] = DL;
    }
}

SDL_Rect Board::getTileSrcRect(TileType type)
{
    SDL_Rect rect = {static_cast<int>(type) * 64, 0, 64, 64};
    return rect;
}

void Board::render(int x, int y)
{
    for (int i = 0; i < 15; ++i)
    {
        for (int j = 0; j < 15; ++j)
        {
            SDL_Rect src = getTileSrcRect(boardLayout[i][j]);
            SDL_Rect dst = {x + j * 64, y + i * 64, 64, 64};
            SDL_RenderCopy(renderer, tilesetTexture, &src, &dst);

            if (placedTiles[i][j])
            {
                placedTiles[i][j]->render(dst.x, dst.y);
            }
        }
    }
}

void Board::placeTile(int boardX, int boardY, const Tile &tile)
{
    if (placedTiles[boardY][boardX] == nullptr)
    {                                                 // Note: rows are Y, columns X
        placedTiles[boardY][boardX] = new Tile(tile); // Copy tile
        placedTiles[boardY][boardX]->setPosition(boardX * 64, boardY * 64);
    }
}
