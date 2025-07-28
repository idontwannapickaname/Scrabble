#include "ScrabbleGame.h"
#include <iostream>
#include <algorithm>
#include <random>

ScrabbleGame::ScrabbleGame(SDL_Renderer* renderer) 
    : m_renderer(renderer), m_currentPlayer(0), m_gameRunning(true), m_selectedTile(-1), 
      m_selectedBoardX(-1), m_selectedBoardY(-1), m_isPlacingTile(false),
      m_tileFont(nullptr), m_boardTileFont(nullptr), m_scoreFont(nullptr) {
    
    // Initialize scores
    m_playerScores[0] = 0;
    m_playerScores[1] = 0;
    
    // Initialize colors
    m_boardColor = {139, 69, 19, 255};    // Brown
    m_tileColor = {255, 248, 220, 255};   // Cream
    m_textColor = {0, 0, 0, 255};         // Black
    m_selectedColor = {255, 255, 0, 255}; // Yellow
    
    // Initialize tile values
    m_tileValues = {
        {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
        {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
        {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
        {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}
    };
    
    std::cout << "ScrabbleGame initialized!" << std::endl;
}

ScrabbleGame::~ScrabbleGame() {
    if (m_tileFont) {
        TTF_CloseFont(m_tileFont);
    }
    if (m_boardTileFont) {
        TTF_CloseFont(m_boardTileFont);
    }
    if (m_scoreFont) {
        TTF_CloseFont(m_scoreFont);
    }
}

bool ScrabbleGame::init() {
    // Load fonts
    m_tileFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 24);
    if (!m_tileFont) {
        m_tileFont = TTF_OpenFont("arial.ttf", 24);
    }
    
    m_boardTileFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 16);
    if (!m_boardTileFont) {
        m_boardTileFont = TTF_OpenFont("arial.ttf", 16);
    }
    
    m_scoreFont = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 18);
    if (!m_scoreFont) {
        m_scoreFont = TTF_OpenFont("arial.ttf", 18);
    }
    
    // Initialize game
    initializeBoard();
    initializeTileBag();
    refillPlayerTiles();
    
    // Set up UI rectangles
    m_boardRect = {50, 50, 450, 450};
    m_scoreRect = {520, 50, 200, 150};
    
    for (int i = 0; i < 7; ++i) {
        m_tileRects[i] = {50 + i * 60, 520, 50, 50};
    }
    
    std::cout << "ScrabbleGame initialized successfully!" << std::endl;
    return true;
}

void ScrabbleGame::initializeBoard() {
    // Clear board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            m_board[i][j] = ' ';
            m_boardMultipliers[i][j] = 1;
        }
    }
    
    // Set up special squares (simplified)
    // Center star
    m_boardMultipliers[7][7] = 2;
    
    // Some double word squares
    for (int i = 0; i < BOARD_SIZE; i += 7) {
        m_boardMultipliers[i][i] = 2;
        m_boardMultipliers[i][BOARD_SIZE-1-i] = 2;
    }
}

void ScrabbleGame::initializeTileBag() {
    m_tileBag.clear();
    
    // Standard Scrabble tile distribution
    std::string tiles = "AAAAAAAAABBCCDDDDEEEEEEEEEEEEFFFGGHHIIIIIIIIIJKLLLLMMNNNNNNOOOOOOOOPPQRRRRRRSSSSTTTTTTUUUUVVWWXYYZ";
    
    for (char tile : tiles) {
        m_tileBag.push_back(tile);
    }
    
    // Shuffle the bag
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_tileBag.begin(), m_tileBag.end(), g);
}

void ScrabbleGame::refillPlayerTiles() {
    while (m_playerTiles.size() < 7 && !m_tileBag.empty()) {
        m_playerTiles.push_back(m_tileBag.back());
        m_tileBag.pop_back();
    }
}

void ScrabbleGame::run() {
    while (m_gameRunning) {
        handleInput();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void ScrabbleGame::handleInput() {
    // This method is now empty since we handle events through handleEvent
}

void ScrabbleGame::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            m_gameRunning = false;
            break;
            
        case SDL_KEYDOWN:
            std::cout << "Key pressed: " << event.key.keysym.sym << std::endl;
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    m_gameRunning = false;
                    break;
                    
                case SDLK_LEFT:
                    if (m_selectedTile >= 0) {
                        m_selectedTile = (m_selectedTile - 1 + 7) % 7;
                        std::cout << "Selected tile: " << m_selectedTile << std::endl;
                    }
                    break;
                    
                case SDLK_RIGHT:
                    if (m_selectedTile >= 0) {
                        m_selectedTile = (m_selectedTile + 1) % 7;
                        std::cout << "Selected tile: " << m_selectedTile << std::endl;
                    }
                    break;
                    
                case SDLK_SPACE:
                    std::cout << "Space pressed!" << std::endl;
                    // Allow toggling placing mode even without tile selected
                    m_isPlacingTile = !m_isPlacingTile;
                    std::cout << "Placing tile mode: " << (m_isPlacingTile ? "ON" : "OFF") << std::endl;
                    break;
                    
                case SDLK_RETURN:
                    std::cout << "Enter pressed!" << std::endl;
                    // Place tile logic
                    if (m_isPlacingTile && m_selectedBoardX >= 0 && m_selectedBoardY >= 0) {
                        if (m_selectedTile >= 0 && m_selectedTile < m_playerTiles.size()) {
                            std::cout << "Placing tile " << m_playerTiles[m_selectedTile] << " at (" 
                                     << m_selectedBoardX << "," << m_selectedBoardY << ")" << std::endl;
                            placeTile(m_selectedBoardX, m_selectedBoardY, m_playerTiles[m_selectedTile]);
                            m_playerTiles.erase(m_playerTiles.begin() + m_selectedTile);
                            refillPlayerTiles();
                            m_selectedTile = -1;
                            m_isPlacingTile = false;
                        } else {
                            std::cout << "No tile selected! Please select a tile first." << std::endl;
                        }
                    } else {
                        std::cout << "Cannot place tile - placing mode: " << m_isPlacingTile 
                                 << ", board pos: (" << m_selectedBoardX << "," << m_selectedBoardY << ")" << std::endl;
                        if (!m_isPlacingTile) {
                            std::cout << "Press SPACE to enable placing mode" << std::endl;
                        }
                        if (m_selectedBoardX < 0 || m_selectedBoardY < 0) {
                            std::cout << "Please click on board to select position" << std::endl;
                        }
                    }
                    break;
            }
            break;
            
                    case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    
                    std::cout << "Mouse click at (" << mouseX << "," << mouseY << ")" << std::endl;
                    
                    // Check if clicking on board
                    if (mouseX >= m_boardRect.x && mouseX < m_boardRect.x + m_boardRect.w &&
                        mouseY >= m_boardRect.y && mouseY < m_boardRect.y + m_boardRect.h) {
                        
                        int boardX = (mouseX - m_boardRect.x) / 30;
                        int boardY = (mouseY - m_boardRect.y) / 30;
                        
                        if (boardX >= 0 && boardX < BOARD_SIZE && boardY >= 0 && boardY < BOARD_SIZE) {
                            m_selectedBoardX = boardX;
                            m_selectedBoardY = boardY;
                            std::cout << "Selected board position: (" << boardX << "," << boardY << ")" << std::endl;
                            
                            // Auto-place tile if we have a selected tile
                            if (m_selectedTile >= 0 && m_selectedTile < m_playerTiles.size()) {
                                std::cout << "Auto-placing tile " << m_playerTiles[m_selectedTile] << " at (" 
                                         << boardX << "," << boardY << ")" << std::endl;
                                placeTile(boardX, boardY, m_playerTiles[m_selectedTile]);
                                m_playerTiles.erase(m_playerTiles.begin() + m_selectedTile);
                                refillPlayerTiles();
                                m_selectedTile = -1;
                            }
                        }
                    }
                    
                    // Check if clicking on player tiles
                    for (int i = 0; i < 7; ++i) {
                        if (mouseX >= m_tileRects[i].x && mouseX < m_tileRects[i].x + m_tileRects[i].w &&
                            mouseY >= m_tileRects[i].y && mouseY < m_tileRects[i].y + m_tileRects[i].h) {
                            m_selectedTile = i;
                            std::cout << "Selected tile " << i << ": " << m_playerTiles[i] << std::endl;
                            break;
                        }
                    }
                }
                break;
    }
}

void ScrabbleGame::update() {
    // Game logic updates
}

void ScrabbleGame::render() {
    // Clear screen with light background
    SDL_SetRenderDrawColor(m_renderer, 240, 240, 240, 255);
    SDL_RenderClear(m_renderer);
    
    drawBoard();
    drawPlayerTiles();
    drawScores();
    
    SDL_RenderPresent(m_renderer);
}

void ScrabbleGame::drawBoard() {
    // Draw board background
    SDL_SetRenderDrawColor(m_renderer, m_boardColor.r, m_boardColor.g, m_boardColor.b, m_boardColor.a);
    SDL_RenderFillRect(m_renderer, &m_boardRect);
    
    // Draw grid lines
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(m_renderer, 
                          m_boardRect.x + i * 30, m_boardRect.y,
                          m_boardRect.x + i * 30, m_boardRect.y + m_boardRect.h);
        SDL_RenderDrawLine(m_renderer, 
                          m_boardRect.x, m_boardRect.y + i * 30,
                          m_boardRect.x + m_boardRect.w, m_boardRect.y + i * 30);
    }
    
    // Draw tiles on board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (m_board[i][j] != ' ') {
                // Center tile in board cell (30x30 cell, 28x28 tile)
                int tileX = m_boardRect.x + j * 30 + 1;
                int tileY = m_boardRect.y + i * 30 + 1;
                drawTile(tileX, tileY, m_board[i][j]);
            }
        }
    }
    
    // Highlight selected board position
    if (m_selectedBoardX >= 0 && m_selectedBoardY >= 0) {
        SDL_Rect highlightRect = {
            m_boardRect.x + m_selectedBoardX * 30,
            m_boardRect.y + m_selectedBoardY * 30,
            30, 30
        };
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 128);
        SDL_RenderFillRect(m_renderer, &highlightRect);
    }
}

void ScrabbleGame::drawPlayerTiles() {
    for (int i = 0; i < 7; ++i) {
        if (i < m_playerTiles.size()) {
            bool selected = (i == m_selectedTile);
            
            // Draw larger tiles for player tiles
            SDL_Rect tileRect = {m_tileRects[i].x + 5, m_tileRects[i].y + 5, 40, 40};
            
            // Draw tile background
            SDL_Color bgColor = selected ? m_selectedColor : m_tileColor;
            SDL_SetRenderDrawColor(m_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(m_renderer, &tileRect);
            
            // Draw tile border
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(m_renderer, &tileRect);
            
            // Draw tile letter
            if (m_tileFont) {
                std::string tileStr(1, m_playerTiles[i]);
                drawText(m_tileRects[i].x + 15, m_tileRects[i].y + 10, tileStr, m_tileFont, m_textColor);
                
                // Draw tile value
                std::string valueStr = std::to_string(m_tileValues[m_playerTiles[i]]);
                drawText(m_tileRects[i].x + 30, m_tileRects[i].y + 25, valueStr, m_tileFont, {128, 128, 128, 255});
            }
        }
    }
}

void ScrabbleGame::drawScores() {
    // Draw score background
    SDL_SetRenderDrawColor(m_renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(m_renderer, &m_scoreRect);
    
    // Draw scores and game info
    std::string scoreText = "Player 1: " + std::to_string(m_playerScores[0]) + 
                           "\nPlayer 2: " + std::to_string(m_playerScores[1]) +
                           "\nCurrent: Player " + std::to_string(m_currentPlayer + 1) +
                           "\nSelected Tile: " + (m_selectedTile >= 0 ? std::to_string(m_selectedTile) : "None") +
                           "\n\nControls:" +
                           "\nClick tile then click board" +
                           "\nSpace: Toggle placing mode" +
                           "\nEnter: Place tile manually";
    
    if (m_scoreFont) {
        drawText(m_scoreRect.x + 10, m_scoreRect.y + 10, scoreText, m_scoreFont, m_textColor);
    }
}

void ScrabbleGame::drawTile(int x, int y, char tile, bool selected) {
    SDL_Rect tileRect = {x, y, 28, 28}; // Smaller tile size to fit in board cells
    
    // Draw tile background
    SDL_Color bgColor = selected ? m_selectedColor : m_tileColor;
    SDL_SetRenderDrawColor(m_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(m_renderer, &tileRect);
    
    // Draw tile border
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(m_renderer, &tileRect);
    
    // Draw tile letter with smaller font
    if (m_boardTileFont) {
        std::string tileStr(1, tile);
        drawText(x + 8, y + 4, tileStr, m_boardTileFont, m_textColor);
        
        // Draw tile value (smaller)
        std::string valueStr = std::to_string(m_tileValues[tile]);
        drawText(x + 18, y + 18, valueStr, m_boardTileFont, {128, 128, 128, 255});
    }
}

void ScrabbleGame::drawText(int x, int y, const std::string& text, TTF_Font* font, SDL_Color color) {
    if (!font) return;
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
        if (textTexture) {
            SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
            SDL_RenderCopy(m_renderer, textTexture, nullptr, &textRect);
            SDL_DestroyTexture(textTexture);
        }
        SDL_FreeSurface(textSurface);
    }
}

void ScrabbleGame::placeTile(int boardX, int boardY, char tile) {
    if (boardX >= 0 && boardX < BOARD_SIZE && boardY >= 0 && boardY < BOARD_SIZE) {
        if (m_board[boardY][boardX] == ' ') {
            m_board[boardY][boardX] = tile;
            
            // Calculate and add score
            int score = m_tileValues[tile] * m_boardMultipliers[boardY][boardX];
            m_playerScores[m_currentPlayer] += score;
            
            // Switch players
            m_currentPlayer = (m_currentPlayer + 1) % 2;
        }
    }
}

bool ScrabbleGame::isValidPlacement(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && m_board[y][x] == ' ';
}

int ScrabbleGame::calculateScore(const std::string& word) {
    int score = 0;
    for (char c : word) {
        score += m_tileValues[c];
    }
    return score;
} 