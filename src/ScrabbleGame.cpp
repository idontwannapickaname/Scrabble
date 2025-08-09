#include "ScrabbleGame.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono> // Added for time-based seed

ScrabbleGame::ScrabbleGame(SDL_Renderer* renderer)
    : m_renderer(renderer), m_gameRunning(true), m_selectedTile(-1),
      m_selectedBoardX(-1), m_selectedBoardY(-1), m_isPlacingTile(false),
      m_tileFont(nullptr), m_boardTileFont(nullptr), m_scoreFont(nullptr) {

    // Initialize single player score
    m_playerScore = 0;

    // Initialize colors
    m_boardColor = {139, 69, 19, 255};    // Brown
    m_tileColor = {255, 248, 220, 255};   // Cream
    m_textColor = {0, 0, 0, 255};         // Black
    m_selectedColor = {255, 255, 0, 255}; // Yellow
    m_quitButtonColor = {220, 20, 60, 255}; // Crimson red for quit button

    // Initialize tile values
    m_tileValues = {
        {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
        {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
        {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
        {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}
    };
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

    // Initialize game board only
    initializeBoard();

    // Set up UI rectangles
    m_boardRect = {50, 50, 600, 600};  // Increased board size from 450x450 to 600x600
    m_scoreRect = {0, 0, 0, 0};  // Hide score panel

    for (int i = 0; i < 7; ++i) {
        m_tileRects[i] = {50 + i * 60, 670, 50, 50};  // Move tiles below board
    }
    
    // Initialize quit button
    m_quitButtonRect = {650, 50, 100, 40};  // Top right corner

    return true;
}

void ScrabbleGame::initializeBoard() {
    // Clear board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            m_board[i][j] = ' ';
            m_boardMultipliers[i][j] = 1;
            m_boardWordMultipliers[i][j] = false;
        }
    }

    // Triple Word Score (Orange) - 8 positions
    int tripleWordPositions[8][2] = {
        {0, 0}, {0, 7}, {0, 14},  // Top row
        {7, 0}, {7, 14},           // Middle rows
        {14, 0}, {14, 7}, {14, 14} // Bottom row
    };
    
    for (int i = 0; i < 8; ++i) {
        int row = tripleWordPositions[i][0];
        int col = tripleWordPositions[i][1];
        m_boardMultipliers[row][col] = 3;
        m_boardWordMultipliers[row][col] = true;
    }

    // Double Word Score (Pink/Magenta) - 16 positions
    int doubleWordPositions[][2] = {
        {1, 1}, {2, 2}, {3, 3}, {4, 4},
        {1, 13}, {2, 12}, {3, 11}, {4, 10},
        {13, 1}, {12, 2}, {11, 3}, {10, 4},
        {13, 13}, {12, 12}, {11, 11}, {10, 10}
    };
    
    for (int i = 0; i < 16; ++i) {
        int row = doubleWordPositions[i][0];
        int col = doubleWordPositions[i][1];
        if (row < BOARD_SIZE && col < BOARD_SIZE) {
            m_boardMultipliers[row][col] = 2;
            m_boardWordMultipliers[row][col] = true;
        }
    }

    // Triple Letter Score (Dark Blue) - 22 positions
    int tripleLetterPositions[][2] = {
        {1, 5}, {1, 9},
        {5, 1}, {5, 5}, {5, 9}, {5, 13},
        {9, 1}, {9, 5}, {9, 9}, {9, 13},
        {13, 5}, {13, 9},
        {3, 7}, {11, 7},
        {7, 3}, {7, 11}
    };
    
    for (int i = 0; i < 16; ++i) {
        int row = tripleLetterPositions[i][0];
        int col = tripleLetterPositions[i][1];
        if (row < BOARD_SIZE && col < BOARD_SIZE) {
            m_boardMultipliers[row][col] = 3;
            m_boardWordMultipliers[row][col] = false;
        }
    }

    // Double Letter Score (Light Blue/Cyan) - 24 positions
    int doubleLetterPositions[][2] = {
        {0, 3}, {0, 11},
        {2, 6}, {2, 8},
        {3, 0}, {3, 7}, {3, 14},
        {6, 2}, {6, 6}, {6, 8}, {6, 12},
        {8, 2}, {8, 6}, {8, 8}, {8, 12},
        {11, 0}, {11, 7}, {11, 14},
        {12, 6}, {12, 8},
        {14, 3}, {14, 11}
    };
    
    for (int i = 0; i < 24; ++i) {
        int row = doubleLetterPositions[i][0];
        int col = doubleLetterPositions[i][1];
        if (row < BOARD_SIZE && col < BOARD_SIZE && m_boardMultipliers[row][col] == 1) {
            m_boardMultipliers[row][col] = 2;
            m_boardWordMultipliers[row][col] = false;
        }
    }

    // Center star (starting point) - Double Word
    m_boardMultipliers[7][7] = 2;
    m_boardWordMultipliers[7][7] = true;
}

void ScrabbleGame::initializeTileBag() {
    m_tileBag.clear();

    // Standard Scrabble tile distribution
    std::string tiles = "AAAAAAAAABBCCDDDDEEEEEEEEEEEEFFFGGHHIIIIIIIIIJKLLLLMMNNNNNNOOOOOOOOPPQRRRRRRSSSSTTTTTTUUUUVVWWXYYZ";

    for (char tile : tiles) {
        m_tileBag.push_back(tile);
    }

    // Shuffle the bag with time-based seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 g(seed);
    std::shuffle(m_tileBag.begin(), m_tileBag.end(), g);
    
}

void ScrabbleGame::refillPlayerTiles() {
    // Clear current tiles
    m_playerTiles.clear();
    
    // Define vowels
    std::vector<char> vowels = {'A', 'E', 'I', 'O', 'U'};
    std::vector<char> consonants;
    
    // Separate vowels and consonants from tile bag
    std::vector<char> tempBag = m_tileBag;
    for (char tile : tempBag) {
        if (std::find(vowels.begin(), vowels.end(), tile) != vowels.end()) {
            // This is a vowel, keep it in tile bag for now
        } else {
            consonants.push_back(tile);
        }
    }
    
    // Ensure we have at least 3 vowels
    int vowelCount = 0;
    for (int i = 0; i < 3; ++i) {
        // Find a vowel in the tile bag
        bool foundVowel = false;
        for (auto it = m_tileBag.begin(); it != m_tileBag.end(); ++it) {
            if (std::find(vowels.begin(), vowels.end(), *it) != vowels.end()) {
                m_playerTiles.push_back(*it);
                m_tileBag.erase(it);
                vowelCount++;
                foundVowel = true;
                break;
            }
        }
        if (!foundVowel) {
            break;
        }
    }
    
    // Fill remaining slots with random tiles
    while (m_playerTiles.size() < 7 && !m_tileBag.empty()) {
        m_playerTiles.push_back(m_tileBag.back());
        m_tileBag.pop_back();
    }
    
}

void ScrabbleGame::resetGame() {
    // Reset player score
    m_playerScore = 0;
    
    // Clear player tiles
    m_playerTiles.clear();
    
    // Reset board
    initializeBoard();
    
    // Create new tile bag with fresh tiles
    initializeTileBag();
    
    // Give player new tiles
    refillPlayerTiles();
    
    // Reset selection
    m_selectedTile = -1;
    m_selectedBoardX = -1;
    m_selectedBoardY = -1;
    m_isPlacingTile = false;
    
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
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    m_gameRunning = false;
                    break;

                case SDLK_LEFT:
                    if (m_selectedTile >= 0) {
                        m_selectedTile = (m_selectedTile - 1 + 7) % 7;
                    }
                    break;

                case SDLK_RIGHT:
                    if (m_selectedTile >= 0) {
                        m_selectedTile = (m_selectedTile + 1) % 7;
                    }
                    break;

                case SDLK_SPACE:
                    // Allow toggling placing mode even without tile selected
                    m_isPlacingTile = !m_isPlacingTile;
                    break;

                case SDLK_RETURN:
                    // Place tile logic
                    if (m_isPlacingTile && m_selectedBoardX >= 0 && m_selectedBoardY >= 0) {
                        if (m_selectedTile >= 0 && m_selectedTile < m_playerTiles.size()) {
                            placeTile(m_selectedBoardX, m_selectedBoardY, m_playerTiles[m_selectedTile]);
                            m_playerTiles.erase(m_playerTiles.begin() + m_selectedTile);
                            // Don't refill tiles - only show remaining tiles
                            m_selectedTile = -1;
                            m_isPlacingTile = false;
                        }
                    }
                    break;
            }
            break;

                    case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    // Check if quit button was clicked
                    if (isQuitButtonClicked(mouseX, mouseY)) {
                        m_gameRunning = false;
                        break; // Break out of the switch instead of return
                    }

                    // Check if clicking on board
                    if (mouseX >= m_boardRect.x && mouseX < m_boardRect.x + m_boardRect.w &&
                        mouseY >= m_boardRect.y && mouseY < m_boardRect.y + m_boardRect.h) {

                        int boardX = (mouseX - m_boardRect.x) / 40;  // 40px per cell
                        int boardY = (mouseY - m_boardRect.y) / 40;

                        if (boardX >= 0 && boardX < BOARD_SIZE && boardY >= 0 && boardY < BOARD_SIZE) {
                            m_selectedBoardX = boardX;
                            m_selectedBoardY = boardY;

                            // Auto-place tile if we have a selected tile
                            if (m_selectedTile >= 0 && m_selectedTile < m_playerTiles.size()) {
                                placeTile(boardX, boardY, m_playerTiles[m_selectedTile]);
                                m_playerTiles.erase(m_playerTiles.begin() + m_selectedTile);
                                // Don't refill tiles - only show remaining tiles
                                m_selectedTile = -1;
                            }
                        }
                    }

                    // Check if clicking on player tiles
                    for (int i = 0; i < 7; ++i) {
                        if (mouseX >= m_tileRects[i].x && mouseX < m_tileRects[i].x + m_tileRects[i].w &&
                            mouseY >= m_tileRects[i].y && mouseY < m_tileRects[i].y + m_tileRects[i].h) {
                            m_selectedTile = i;
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
    drawQuitButton(); // Add quit button rendering
    
    SDL_RenderPresent(m_renderer);
}

void ScrabbleGame::drawBoard() {
    // Draw board background
    SDL_SetRenderDrawColor(m_renderer, m_boardColor.r, m_boardColor.g, m_boardColor.b, m_boardColor.a);
    SDL_RenderFillRect(m_renderer, &m_boardRect);

    // Draw special squares with colors and labels
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            int cellX = m_boardRect.x + j * 40;
            int cellY = m_boardRect.y + i * 40;
            SDL_Rect cellRect = {cellX, cellY, 40, 40};
            
            // Draw special square colors
            if (m_boardMultipliers[i][j] == 3) {
                if (m_boardWordMultipliers[i][j]) {
                    // Triple Word Score (Orange)
                    SDL_SetRenderDrawColor(m_renderer, 255, 165, 0, 255);
                    SDL_RenderFillRect(m_renderer, &cellRect);
                    
                    // Draw label
                    if (m_scoreFont) {
                        drawText(cellX + 2, cellY + 2, "TW", m_scoreFont, {255, 255, 255, 255});
                    }
                } else {
                    // Triple Letter Score (Dark Blue)
                    SDL_SetRenderDrawColor(m_renderer, 0, 0, 150, 255);
                    SDL_RenderFillRect(m_renderer, &cellRect);
                    
                    // Draw label
                    if (m_scoreFont) {
                        drawText(cellX + 2, cellY + 2, "TL", m_scoreFont, {255, 255, 255, 255});
                    }
                }
            } else if (m_boardMultipliers[i][j] == 2) {
                if (m_boardWordMultipliers[i][j]) {
                    // Double Word Score (Pink/Magenta)
                    SDL_SetRenderDrawColor(m_renderer, 255, 100, 100, 255);
                    SDL_RenderFillRect(m_renderer, &cellRect);
                    
                    // Draw label
                    if (m_scoreFont) {
                        drawText(cellX + 2, cellY + 2, "DW", m_scoreFont, {255, 255, 255, 255});
                    }
                } else {
                    // Double Letter Score (Light Blue/Cyan)
                    SDL_SetRenderDrawColor(m_renderer, 173, 216, 230, 255);
                    SDL_RenderFillRect(m_renderer, &cellRect);
                    
                    // Draw label
                    if (m_scoreFont) {
                        drawText(cellX + 2, cellY + 2, "DL", m_scoreFont, {0, 0, 0, 255});
                    }
                }
            }
        }
    }

    // Draw grid lines
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(m_renderer,
                          m_boardRect.x + i * 40, m_boardRect.y,  // 40px per cell (600/15)
                          m_boardRect.x + i * 40, m_boardRect.y + m_boardRect.h);
        SDL_RenderDrawLine(m_renderer,
                          m_boardRect.x, m_boardRect.y + i * 40,
                          m_boardRect.x + m_boardRect.w, m_boardRect.y + i * 40);
    }

    // Draw center star (5-pointed star)
    int centerX = m_boardRect.x + 7 * 40 + 20;
    int centerY = m_boardRect.y + 7 * 40 + 20;
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    
    // Draw a simple star pattern
    SDL_RenderDrawLine(m_renderer, centerX, centerY - 8, centerX, centerY + 8);
    SDL_RenderDrawLine(m_renderer, centerX - 8, centerY, centerX + 8, centerY);
    SDL_RenderDrawLine(m_renderer, centerX - 6, centerY - 6, centerX + 6, centerY + 6);
    SDL_RenderDrawLine(m_renderer, centerX - 6, centerY + 6, centerX + 6, centerY - 6);

    // Draw tiles on board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (m_board[i][j] != ' ') {
                // Center tile in board cell (40x40 cell, 38x38 tile)
                int tileX = m_boardRect.x + j * 40 + 1;
                int tileY = m_boardRect.y + i * 40 + 1;
                drawTile(tileX, tileY, m_board[i][j]);
            }
        }
    }

    // Highlight selected board position
    if (m_selectedBoardX >= 0 && m_selectedBoardY >= 0) {
        SDL_Rect highlightRect = {
            m_boardRect.x + m_selectedBoardX * 40,
            m_boardRect.y + m_selectedBoardY * 40,
            40, 40
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
    std::string scoreText = "Score: " + std::to_string(m_playerScore) +
                           "\nTiles Left: " + std::to_string(m_playerTiles.size()) +
                           "\nSelected Tile: " + (m_selectedTile >= 0 ? std::to_string(m_selectedTile) : "None") +
                           "\n\nControls:" +
                           "\nClick tile then click board" +
                           "\nSpace: Toggle placing mode" +
                           "\nEnter: Place tile manually";

    if (m_scoreFont) {
        drawText(m_scoreRect.x + 10, m_scoreRect.y + 10, scoreText, m_scoreFont, m_textColor);
    }
}

void ScrabbleGame::drawQuitButton() {
    // Draw quit button background
    SDL_SetRenderDrawColor(m_renderer, 
                          m_quitButtonColor.r, 
                          m_quitButtonColor.g, 
                          m_quitButtonColor.b, 
                          m_quitButtonColor.a);
    SDL_RenderFillRect(m_renderer, &m_quitButtonRect);
    
    // Draw quit button text
    if (m_scoreFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_scoreFont, "QUIT", m_textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    m_quitButtonRect.x + (m_quitButtonRect.w - textSurface->w) / 2,
                    m_quitButtonRect.y + (m_quitButtonRect.h - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                SDL_RenderCopy(m_renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }
}

bool ScrabbleGame::isQuitButtonClicked(int x, int y) {
    return (x >= m_quitButtonRect.x && x <= m_quitButtonRect.x + m_quitButtonRect.w &&
            y >= m_quitButtonRect.y && y <= m_quitButtonRect.y + m_quitButtonRect.h);
}

void ScrabbleGame::drawTile(int x, int y, char tile, bool selected) {
    SDL_Rect tileRect = {x, y, 38, 38}; // Larger tile size to fit in 40x40 board cells

    // Draw tile background
    SDL_Color bgColor = selected ? m_selectedColor : m_tileColor;
    SDL_SetRenderDrawColor(m_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(m_renderer, &tileRect);

    // Draw tile border
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(m_renderer, &tileRect);

    // Draw tile letter with larger font
    if (m_boardTileFont) {
        std::string tileStr(1, tile);
        drawText(x + 12, y + 8, tileStr, m_boardTileFont, m_textColor);

        // Draw tile value (smaller)
        std::string valueStr = std::to_string(m_tileValues[tile]);
        drawText(x + 24, y + 24, valueStr, m_boardTileFont, {128, 128, 128, 255});
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
            m_playerScore += score;

            // No player switching needed for single player
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
