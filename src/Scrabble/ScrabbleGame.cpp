#include "ScrabbleGame.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono> // Added for time-based seed
#include <cstdio> // For FILE operations

ScrabbleGame::ScrabbleGame(SDL_Renderer* renderer)
    : m_renderer(renderer), m_gameRunning(true), m_selectedTile(-1),
      m_selectedBoardX(-1), m_selectedBoardY(-1), m_isPlacingTile(false),
      m_isSwappingTile(false), m_hasSwappedThisTurn(false), m_tileFont(nullptr), m_boardTileFont(nullptr), m_scoreFont(nullptr) {

    // Initialize player scores
    m_player1Score = 0;
    m_player2Score = 0;
    m_currentPlayer = 1; // Start with player 1

    // Initialize player tiles
    m_player1Tiles.clear();
    m_player2Tiles.clear();

    // Initialize colors
    m_boardColor = {139, 69, 19, 255};    // Brown
    m_tileColor = {255, 248, 220, 255};   // Cream
    m_textColor = {0, 0, 0, 255};         // Black
    m_selectedColor = {255, 255, 0, 255}; // Yellow
    m_quitButtonColor = {220, 20, 60, 255}; // Crimson red for quit button
    m_submitButtonColor = {34, 139, 34, 255}; // Forest green for submit button
    m_player1Color = {255, 100, 100, 255}; // Light red for player 1
    m_player2Color = {100, 100, 255, 255}; // Light blue for player 2

    // Initialize tile values
    m_tileValues = {
        {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
        {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
        {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
        {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}
    };

    // Initialize button hover states
    m_quitButtonHovered = false;
    m_submitButtonHovered = false;
    m_undoButtonHovered = false;
    m_skipButtonHovered = false;
    m_swapButtonHovered = false;

    // Initialize error message
    m_errorMessage = "";
    m_showError = false;
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

    // Load dictionary
    loadDictionary();

    // Initialize game board only
    initializeBoard();

    // Set up UI rectangles
    m_boardRect = {50, 50, 600, 600};  // Increased board size from 450x450 to 600x600
    m_scoreRect = {0, 0, 0, 0};  // Hide score panel

    // Player label rectangles
    m_player1Rect = {50, 680, 200, 60};  // Below board, left side
    m_player2Rect = {450, 680, 200, 60}; // Below board, right side

    for (int i = 0; i < 7; ++i) {
        m_tileRects[i] = {50 + i * 60, 750, 50, 50};  // Move tiles below player labels
    }

        // Initialize buttons - 5 buttons thẳng hàng bên phải board
    m_quitButtonRect = {680, 50, 100, 40};      // Top button, cách board 30px
    m_submitButtonRect = {680, 100, 100, 40};   // Second button
    m_undoButtonRect = {680, 150, 100, 40};     // Third button
    m_skipButtonRect = {680, 200, 100, 40};     // Fourth button
    m_swapButtonRect = {680, 250, 100, 40};     // Fifth button

    // Initialize error label area below buttons
    m_errorLabelRect = {700, 300, 180, 80};     // Error label below buttons, adjusted for 900px width

    // Initialize UI elements for click detection
    m_quitButton = {680, 50, 100, 40};          // Quit button
    m_submitButton = {680, 100, 100, 40};       // Submit button
    m_undoButton = {680, 150, 100, 40};         // Undo button
    m_skipButton = {680, 200, 100, 40};         // Skip button
    m_swapButton = {680, 250, 100, 40};         // Swap button

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

void ScrabbleGame::loadDictionary() {
    m_dictionary.clear();

    // Try to load dictionary from assets folder
    std::string dictPath = "assets/dictionary/dictionary.txt";
    FILE* file = fopen(dictPath.c_str(), "r");

    if (file) {
        char word[256];
        while (fgets(word, sizeof(word), file)) {
            // Remove newline and convert to uppercase
            std::string cleanWord = word;
            if (!cleanWord.empty() && cleanWord[cleanWord.length()-1] == '\n') {
                cleanWord.erase(cleanWord.length()-1);
            }

            // Convert to uppercase
            std::transform(cleanWord.begin(), cleanWord.end(), cleanWord.begin(), ::toupper);

            // Only add words with length >= 2
            if (cleanWord.length() >= 2) {
                m_dictionary.insert(cleanWord);
            }
        }
        fclose(file);
        // Dictionary loaded successfully
    } else {
        // If dictionary file not found, add some basic English words
        std::vector<std::string> basicWords = {
            "HELLO", "WORLD", "GAME", "PLAY", "WORD", "LETTER", "SCORE", "BOARD",
            "TILE", "PLAYER", "TURN", "MOVE", "VALID", "INVALID", "DICTIONARY",
            "CAT", "DOG", "BIRD", "FISH", "TREE", "FLOWER", "HOUSE", "CAR",
            "BOOK", "PEN", "PAPER", "WATER", "FIRE", "EARTH", "AIR", "SUN",
            "MOON", "STAR", "CLOUD", "RAIN", "SNOW", "WIND", "STORM", "LIGHT",
            "DARK", "BIG", "SMALL", "FAST", "SLOW", "HOT", "COLD", "WARM",
            "COOL", "GOOD", "BAD", "NEW", "OLD", "YOUNG", "RICH", "POOR",
            "HAPPY", "SAD", "ANGRY", "CALM", "QUIET", "LOUD", "SOFT", "HARD",
            "EASY", "DIFFICULT", "SIMPLE", "COMPLEX", "BEAUTIFUL", "UGLY", "CLEAN", "DIRTY"
        };

        for (const auto& word : basicWords) {
            m_dictionary.insert(word);
        }
        // Using basic dictionary
    }
}

void ScrabbleGame::refillPlayerTiles() {
    // This function is kept for backward compatibility
    refillCurrentPlayerTiles();
}

void ScrabbleGame::refillCurrentPlayerTiles() {
    std::vector<char>& currentTiles = (m_currentPlayer == 1) ? m_player1Tiles : m_player2Tiles;

    // Clear current tiles
    currentTiles.clear();

    // Define vowels
    std::vector<char> vowels = {'A', 'E', 'I', 'O', 'U'};

    // Ensure we have at least 3 vowels
    int vowelCount = 0;
    for (int i = 0; i < 3; ++i) {
        // Find a vowel in the tile bag
        bool foundVowel = false;
        for (auto it = m_tileBag.begin(); it != m_tileBag.end(); ++it) {
            if (std::find(vowels.begin(), vowels.end(), *it) != vowels.end()) {
                currentTiles.push_back(*it);
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
    while (currentTiles.size() < 7 && !m_tileBag.empty()) {
        currentTiles.push_back(m_tileBag.back());
        m_tileBag.pop_back();
    }
}

void ScrabbleGame::resetGame() {
    // Reset player scores
    m_player1Score = 0;
    m_player2Score = 0;
    m_currentPlayer = 1;

    // Clear player tiles
    m_player1Tiles.clear();
    m_player2Tiles.clear();

    // Clear current turn tiles
    m_currentTurnTiles.clear();

    // Reset board
    initializeBoard();

    // Create new tile bag with fresh tiles
    initializeTileBag();

    // Give both players completely new tiles
    m_currentPlayer = 1;
    m_player1Tiles.clear();
    refillCurrentPlayerTiles(); // Give tiles to player 1

    m_currentPlayer = 2;
    m_player2Tiles.clear();
    refillCurrentPlayerTiles(); // Give tiles to player 2

    m_currentPlayer = 1; // Start with player 1

    // Reset selection
    m_selectedTile = -1;
    m_selectedBoardX = -1;
    m_selectedBoardY = -1;
    m_isPlacingTile = false;
    m_isSwappingTile = false;
    m_hasSwappedThisTurn = false;

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
                            m_isSwappingTile = false; // Exit swap mode when entering place mode
                            break;

                        case SDLK_TAB:
                            // Toggle swap mode - only if haven't swapped this turn
                            if (m_selectedTile >= 0 && !m_hasSwappedThisTurn) {
                                m_isSwappingTile = !m_isSwappingTile;
                                m_isPlacingTile = false; // Exit place mode when entering swap mode
                            }
                            break;

                        case SDLK_RETURN:
                            // Place tile logic
                            if (m_isPlacingTile && m_selectedBoardX >= 0 && m_selectedBoardY >= 0) {
                                const std::vector<char>& currentTiles = (m_currentPlayer == 1) ? m_player1Tiles : m_player2Tiles;
                                if (m_selectedTile >= 0 && m_selectedTile < static_cast<int>(currentTiles.size())) {
                                    placeTile(m_selectedBoardX, m_selectedBoardY, currentTiles[m_selectedTile]);
                                    // Remove tile from current player's tiles
                                    if (m_currentPlayer == 1) {
                                        m_player1Tiles.erase(m_player1Tiles.begin() + m_selectedTile);
                                    } else {
                                        m_player2Tiles.erase(m_player2Tiles.begin() + m_selectedTile);
                                    }
                                    // Don't refill tiles - only show remaining tiles
                                    m_selectedTile = -1;
                                    m_isPlacingTile = false;
                                }
                            }
                            break;
                    }
                    break;

        case SDL_MOUSEMOTION:
            // Handle button hover states
            m_quitButtonHovered = isQuitButtonClicked(event.motion.x, event.motion.y);
            m_submitButtonHovered = isSubmitButtonClicked(event.motion.x, event.motion.y);
            m_undoButtonHovered = isUndoButtonClicked(event.motion.x, event.motion.y);
            m_skipButtonHovered = isSkipButtonClicked(event.motion.x, event.motion.y);
            m_swapButtonHovered = isSwapButtonClicked(event.motion.x, event.motion.y);
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

                // Check if submit button was clicked
                if (isSubmitButtonClicked(mouseX, mouseY)) {
                    if (submitTurn()) {
                        switchPlayer(); // Switch to next player only if turn is valid
                    }
                    break;
                }

                // Check if undo button was clicked
                if (isUndoButtonClicked(mouseX, mouseY)) {
                    undoLastMove();
                    break;
                }

                // Check if skip button was clicked
                if (isSkipButtonClicked(mouseX, mouseY)) {
                    skipTurn();
                    break;
                }

                // Check if swap button was clicked
                if (isSwapButtonClicked(mouseX, mouseY)) {
                    if (m_selectedTile >= 0 && !m_hasSwappedThisTurn) {
                        swapSelectedTile();
                    }
                    break;
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
                        const std::vector<char>& currentTiles = (m_currentPlayer == 1) ? m_player1Tiles : m_player2Tiles;
                        if (m_selectedTile >= 0 && m_selectedTile < static_cast<int>(currentTiles.size())) {
                            placeTile(boardX, boardY, currentTiles[m_selectedTile]);
                            // Remove tile from current player's tiles
                            if (m_currentPlayer == 1) {
                                m_player1Tiles.erase(m_player1Tiles.begin() + m_selectedTile);
                            } else {
                                m_player2Tiles.erase(m_player2Tiles.begin() + m_selectedTile);
                            }
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
    drawPlayerLabels(); // Add player labels
    drawQuitButton(); // Add quit button rendering
    drawSubmitButton(); // Add submit button rendering
    drawUndoButton();  // Add undo button rendering
    drawSkipButton();  // Add skip button rendering
    drawSwapButton();  // Add swap button rendering
    drawErrorLabel();  // Add error label rendering

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

    // Draw turn information
    if (m_scoreFont) {
        std::string turnInfo = "Player " + std::to_string(m_currentPlayer) + "'s Turn";
        if (!m_currentTurnTiles.empty()) {
            turnInfo += " (" + std::to_string(m_currentTurnTiles.size()) + " tiles placed)";
        }
        if (m_isSwappingTile) {
            turnInfo += " [SWAP MODE]";
        }
        if (m_hasSwappedThisTurn) {
            turnInfo += " (Swapped)";
        }
        drawText(50, 20, turnInfo, m_scoreFont, m_currentPlayer == 1 ? m_player1Color : m_player2Color);
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
    // Get current player's tiles
    const std::vector<char>& currentTiles = (m_currentPlayer == 1) ? m_player1Tiles : m_player2Tiles;

    for (int i = 0; i < 7; ++i) {
        if (i < currentTiles.size()) {
            bool selected = (i == m_selectedTile);

            // Draw larger tiles for player tiles
            SDL_Rect tileRect = {m_tileRects[i].x + 5, m_tileRects[i].y + 5, 40, 40};

            // Draw tile background - different color for swap mode
            SDL_Color bgColor;
            if (selected && m_isSwappingTile) {
                bgColor = {255, 165, 0, 255}; // Orange for swap mode
            } else if (selected) {
                bgColor = m_selectedColor;
            } else {
                bgColor = m_tileColor;
            }
            SDL_SetRenderDrawColor(m_renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(m_renderer, &tileRect);

            // Draw tile border
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(m_renderer, &tileRect);

            // Draw tile letter
            if (m_tileFont) {
                std::string tileStr(1, currentTiles[i]);
                drawText(m_tileRects[i].x + 15, m_tileRects[i].y + 10, tileStr, m_tileFont, m_textColor);

                // Draw tile value
                std::string valueStr = std::to_string(m_tileValues[currentTiles[i]]);
                drawText(m_tileRects[i].x + 30, m_tileRects[i].y + 25, valueStr, m_tileFont, {128, 128, 128, 255});
            }
        }
    }
}

void ScrabbleGame::drawScores() {
    // Draw score background
    SDL_SetRenderDrawColor(m_renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(m_renderer, &m_scoreRect);

    // Get current player's tiles
    const std::vector<char>& currentTiles = (m_currentPlayer == 1) ? m_player1Tiles : m_player2Tiles;

    // Draw scores and game info
    std::string scoreText = "Tiles Left: " + std::to_string(currentTiles.size()) +
                           "\nSelected Tile: " + (m_selectedTile >= 0 ? std::to_string(m_selectedTile) : "None") +
                           "\n\nControls:" +
                           "\nClick tile then click board" +
                           "\nSpace: Toggle placing mode" +
                           "\nEnter: Place tile manually";

    if (m_scoreFont) {
        drawText(m_scoreRect.x + 10, m_scoreRect.y + 10, scoreText, m_scoreFont, m_textColor);
    }
}

void ScrabbleGame::drawPlayerLabels() {
    // Draw Player 1 label and score
    SDL_SetRenderDrawColor(m_renderer,
                          m_player1Color.r,
                          m_player1Color.g,
                          m_player1Color.b,
                          m_player1Color.a);
    SDL_RenderFillRect(m_renderer, &m_player1Rect);

    // Draw Player 1 border
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(m_renderer, &m_player1Rect);

    // Draw Player 1 text
    if (m_scoreFont) {
        std::string player1Text = "Player 1: " + std::to_string(m_player1Score);
        if (m_currentPlayer == 1) {
            player1Text += " (Current)";
        }
        drawText(m_player1Rect.x + 10, m_player1Rect.y + 10, player1Text, m_scoreFont, m_textColor);
    }

    // Draw Player 2 label and score
    SDL_SetRenderDrawColor(m_renderer,
                          m_player2Color.r,
                          m_player2Color.g,
                          m_player2Color.b,
                          m_player2Color.a);
    SDL_RenderFillRect(m_renderer, &m_player2Rect);

    // Draw Player 2 border
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(m_renderer, &m_player2Rect);

    // Draw Player 2 text
    if (m_scoreFont) {
        std::string player2Text = "Player 2: " + std::to_string(m_player2Score);
        if (m_currentPlayer == 2) {
            player2Text += " (Current)";
        }
        drawText(m_player2Rect.x + 10, m_player2Rect.y + 10, player2Text, m_scoreFont, m_textColor);
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

void ScrabbleGame::drawSubmitButton() {
    // Draw submit button background
    SDL_SetRenderDrawColor(m_renderer,
                          m_submitButtonColor.r,
                          m_submitButtonColor.g,
                          m_submitButtonColor.b,
                          m_submitButtonColor.a);
    SDL_RenderFillRect(m_renderer, &m_submitButtonRect);

    // Draw submit button text
    if (m_scoreFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_scoreFont, "SUBMIT", m_textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    m_submitButtonRect.x + (m_submitButtonRect.w - textSurface->w) / 2,
                    m_submitButtonRect.y + (m_submitButtonRect.h - textSurface->h) / 2,
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

bool ScrabbleGame::isSubmitButtonClicked(int x, int y) {
    return (x >= m_submitButtonRect.x && x <= m_submitButtonRect.x + m_submitButtonRect.w &&
            y >= m_submitButtonRect.y && y <= m_submitButtonRect.y + m_submitButtonRect.h);
}

void ScrabbleGame::drawUndoButton() {
    // Draw undo button background
    SDL_SetRenderDrawColor(m_renderer,
                          m_undoButtonHovered ? 200 : 150,
                          150,
                          50,
                          255);
    SDL_RenderFillRect(m_renderer, &m_undoButtonRect);

    // Draw undo button text
    if (m_scoreFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_scoreFont, "UNDO", m_textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    m_undoButtonRect.x + (m_undoButtonRect.w - textSurface->w) / 2,
                    m_undoButtonRect.y + (m_undoButtonRect.h - textSurface->h) / 2,
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

bool ScrabbleGame::isUndoButtonClicked(int x, int y) {
    return (x >= m_undoButton.x && x <= m_undoButton.x + m_undoButton.w &&
            y >= m_undoButton.y && y <= m_undoButton.y + m_undoButton.h);
}

void ScrabbleGame::drawSkipButton() {
    // Draw skip button background
    SDL_SetRenderDrawColor(m_renderer,
                          m_skipButtonHovered ? 150 : 100,
                          100,
                          200,
                          255);
    SDL_RenderFillRect(m_renderer, &m_skipButtonRect);

    // Draw skip button text
    if (m_scoreFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_scoreFont, "SKIP", m_textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    m_skipButtonRect.x + (m_skipButtonRect.w - textSurface->w) / 2,
                    m_skipButtonRect.y + (m_skipButtonRect.h - textSurface->h) / 2,
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

bool ScrabbleGame::isSkipButtonClicked(int x, int y) {
    return (x >= m_skipButton.x && x <= m_skipButton.x + m_skipButton.w &&
            y >= m_skipButton.y && y <= m_skipButton.y + m_skipButton.h);
}

void ScrabbleGame::drawSwapButton() {
    // Draw swap button background - grayed out if already swapped
    SDL_Color buttonColor;
    if (m_hasSwappedThisTurn) {
        // Grayed out - already swapped this turn
        buttonColor = {128, 128, 128, 255};
    } else if (m_swapButtonHovered) {
        buttonColor = {200, 100, 200, 255};
    } else {
        buttonColor = {150, 100, 200, 255};
    }

    SDL_SetRenderDrawColor(m_renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(m_renderer, &m_swapButtonRect);

    // Draw swap button text
    if (m_scoreFont) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(m_scoreFont, "SWAP", m_textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {
                    m_swapButtonRect.x + (m_swapButtonRect.w - textSurface->w) / 2,
                    m_swapButtonRect.y + (m_swapButtonRect.h - textSurface->h) / 2,
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

bool ScrabbleGame::isSwapButtonClicked(int x, int y) {
    return (x >= m_swapButton.x && x <= m_swapButton.x + m_swapButton.w &&
            y >= m_swapButton.y && y <= m_swapButton.y + m_swapButton.h);
}

void ScrabbleGame::setErrorMessage(const std::string& message) {
    m_errorMessage = message;
    m_showError = true;
}

void ScrabbleGame::drawErrorLabel() {
    if (!m_showError || m_errorMessage.empty()) {
        return; // Don't draw if no error
    }

    // Draw error label background
    SDL_SetRenderDrawColor(m_renderer, 255, 200, 200, 255); // Light red background
    SDL_RenderFillRect(m_renderer, &m_errorLabelRect);

    // Draw error label border
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255); // Red border
    SDL_RenderDrawRect(m_renderer, &m_errorLabelRect);

    // Draw error message text
    if (m_scoreFont) {
        // Split message into lines for better display
        std::vector<std::string> lines;
        std::string currentLine = "";

        for (char c : m_errorMessage) {
            if (c == '\n') {
                lines.push_back(currentLine);
                currentLine = "";
            } else {
                currentLine += c;
            }
        }
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }

        // Draw each line
        int yOffset = m_errorLabelRect.y + 5;
        for (const auto& line : lines) {
            if (!line.empty()) {
                drawText(m_errorLabelRect.x + 5, yOffset, line, m_scoreFont, {255, 0, 0, 255});
                yOffset += 20; // Line spacing
            }
        }
    }
}

void ScrabbleGame::switchPlayer() {
    m_currentPlayer = (m_currentPlayer == 1) ? 2 : 1;

    // Reset selection for new player
    m_selectedTile = -1;
    m_selectedBoardX = -1;
    m_selectedBoardY = -1;
    m_isPlacingTile = false;
    m_isSwappingTile = false;
    m_hasSwappedThisTurn = false;

    // Clear error message for new player
    m_showError = false;
    m_errorMessage = "";

    // Clear current turn tiles
    m_currentTurnTiles.clear();

    // Clear current player's tiles and give them completely new tiles
    if (m_currentPlayer == 1) {
        m_player1Tiles.clear();
    } else {
        m_player2Tiles.clear();
    }

    // Always give fresh tiles to the new player
    refillCurrentPlayerTiles();
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
            // Track this tile as part of current turn
            m_currentTurnTiles.push_back(std::make_pair(boardY, boardX));
        }
    }
}

bool ScrabbleGame::isValidWord(const std::string& word) {
    return m_dictionary.find(word) != m_dictionary.end();
}

std::vector<std::string> ScrabbleGame::findWordsOnBoard() {
    std::vector<std::string> words;

    // Find horizontal words
    for (int row = 0; row < BOARD_SIZE; ++row) {
        std::vector<std::string> horizontalWords = findWordsInDirection(row, 0, true);
        words.insert(words.end(), horizontalWords.begin(), horizontalWords.end());
    }

    // Find vertical words
    for (int col = 0; col < BOARD_SIZE; ++col) {
        std::vector<std::string> verticalWords = findWordsInDirection(0, col, false);
        words.insert(words.end(), verticalWords.begin(), verticalWords.end());
    }

    return words;
}

std::vector<std::string> ScrabbleGame::findWordsInDirection(int startRow, int startCol, bool horizontal) {
    std::vector<std::string> words;

    if (horizontal) {
        std::string currentWord = "";
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (m_board[startRow][col] != ' ') {
                currentWord += m_board[startRow][col];
            } else {
                if (currentWord.length() >= 2) {
                    words.push_back(currentWord);
                }
                currentWord = "";
            }
        }
        if (currentWord.length() >= 2) {
            words.push_back(currentWord);
        }
    } else {
        std::string currentWord = "";
        for (int row = 0; row < BOARD_SIZE; ++row) {
            if (m_board[row][startCol] != ' ') {
                currentWord += m_board[row][startCol];
            } else {
                if (currentWord.length() >= 2) {
                    words.push_back(currentWord);
                }
                currentWord = "";
            }
        }
        if (currentWord.length() >= 2) {
            words.push_back(currentWord);
        }
    }

    return words;
}

int ScrabbleGame::calculateWordScore(const std::string& word, int row, int col, bool horizontal) {
    int score = 0;
    int wordMultiplier = 1;

    if (horizontal) {
        for (size_t i = 0; i < word.length(); ++i) {
            int currentRow = row;
            int currentCol = col + i;

            if (currentCol < BOARD_SIZE) {
                int letterScore = m_tileValues[word[i]];

                if (m_boardWordMultipliers[currentRow][currentCol]) {
                    wordMultiplier *= m_boardMultipliers[currentRow][currentCol];
                } else {
                    letterScore *= m_boardMultipliers[currentRow][currentCol];
                }

                score += letterScore;
            }
        }
    } else {
        for (size_t i = 0; i < word.length(); ++i) {
            int currentRow = row + i;
            int currentCol = col;

            if (currentRow < BOARD_SIZE) {
                int letterScore = m_tileValues[word[i]];

                if (m_boardWordMultipliers[currentRow][currentCol]) {
                    wordMultiplier *= m_boardMultipliers[currentRow][currentCol];
                } else {
                    letterScore *= m_boardMultipliers[currentRow][currentCol];
                }

                score += letterScore;
            }
        }
    }

    return score * wordMultiplier;
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

bool ScrabbleGame::submitTurn() {
    if (m_currentTurnTiles.empty()) {
        // No tiles placed this turn
        return false;
    }

    // Find words created by the newly placed tiles
    std::vector<std::string> newWords = findNewWords();

    if (newWords.empty()) {
        // No valid words created with new tiles
        return false;
    }

    // Validate all new words - only submit if ALL words are valid
    std::vector<std::string> validWords;
    std::vector<std::string> invalidWords;
    int totalScore = 0;

    for (const auto& word : newWords) {
        if (isValidWord(word)) {
            validWords.push_back(word);
            // Calculate score for this word with proper multipliers
            int wordScore = calculateWordScoreWithPosition(word);
            totalScore += wordScore;
        } else {
            invalidWords.push_back(word);
        }
    }

    // Check if there are any invalid words - if so, reject the turn
    if (!invalidWords.empty()) {
        std::cout << "Cannot submit turn - invalid words found:" << std::endl;
        for (const auto& word : invalidWords) {
            std::cout << "  - '" << word << "' is not in dictionary" << std::endl;
        }
        std::cout << "Please fix invalid words or use UNDO to remove tiles" << std::endl;

        // Set error message for display
        std::string errorMsg = "Unvalid Word:\n";
        for (const auto& word : invalidWords) {
            errorMsg += "- " + word + "\n";
        }
        setErrorMessage(errorMsg);

        return false;
    }

    // All words are valid - proceed with scoring
    if (m_currentPlayer == 1) {
        m_player1Score += totalScore;
    } else {
        m_player2Score += totalScore;
    }

    // Words validated successfully

        // Clear current turn tiles after successful submission
    m_currentTurnTiles.clear();

    // Clear error message on successful submission
    m_showError = false;
    m_errorMessage = "";

    return true;
}

std::vector<std::string> ScrabbleGame::findNewWords() {
    std::vector<std::string> newWords;

    // For each newly placed tile, check for words in both directions
    for (const auto& tilePos : m_currentTurnTiles) {
        int row = tilePos.first;
        int col = tilePos.second;

        // Check horizontal words containing this tile
        std::string horizontalWord = findWordAtPosition(row, col, true);
        if (horizontalWord.length() >= 2 && containsNewTile(horizontalWord, row, col, true)) {
            newWords.push_back(horizontalWord);
        }

        // Check vertical words containing this tile
        std::string verticalWord = findWordAtPosition(row, col, false);
        if (verticalWord.length() >= 2 && containsNewTile(verticalWord, row, col, false)) {
            newWords.push_back(verticalWord);
        }
    }

    // Remove duplicates
    std::sort(newWords.begin(), newWords.end());
    newWords.erase(std::unique(newWords.begin(), newWords.end()), newWords.end());

    return newWords;
}

std::string ScrabbleGame::findWordAtPosition(int row, int col, bool horizontal) {
    std::string word = "";

    if (horizontal) {
        // Find start of word (go left until empty or edge)
        int startCol = col;
        while (startCol >= 0 && m_board[row][startCol] != ' ') {
            startCol--;
        }
        startCol++; // Move back to first letter

        // Build word from start to end
        for (int c = startCol; c < BOARD_SIZE && m_board[row][c] != ' '; c++) {
            word += m_board[row][c];
        }
    } else {
        // Find start of word (go up until empty or edge)
        int startRow = row;
        while (startRow >= 0 && m_board[startRow][col] != ' ') {
            startRow--;
        }
        startRow++; // Move back to first letter

        // Build word from start to end
        for (int r = startRow; r < BOARD_SIZE && m_board[r][col] != ' '; r++) {
            word += m_board[r][col];
        }
    }

    return word;
}

bool ScrabbleGame::containsNewTile(const std::string& word, int row, int col, bool horizontal) {
    // Check if this word contains at least one newly placed tile
    if (horizontal) {
        // Find the word's position on the board
        int startCol = col;
        while (startCol >= 0 && m_board[row][startCol] != ' ') {
            startCol--;
        }
        startCol++;

        // Check if any position in this word contains a new tile
        for (size_t i = 0; i < word.length(); i++) {
            int currentCol = startCol + i;
            if (currentCol < BOARD_SIZE) {
                for (const auto& newTile : m_currentTurnTiles) {
                    if (newTile.first == row && newTile.second == currentCol) {
                        return true;
                    }
                }
            }
        }
    } else {
        // Find the word's position on the board
        int startRow = row;
        while (startRow >= 0 && m_board[startRow][col] != ' ') {
            startRow--;
        }
        startRow++;

        // Check if any position in this word contains a new tile
        for (size_t i = 0; i < word.length(); i++) {
            int currentRow = startRow + i;
            if (currentRow < BOARD_SIZE) {
                for (const auto& newTile : m_currentTurnTiles) {
                    if (newTile.first == currentRow && newTile.second == col) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

int ScrabbleGame::calculateWordScoreWithPosition(const std::string& word) {
    // Find the word's position on the board
    int wordRow = -1, wordCol = -1;
    bool isHorizontal = false;

    // Search for this word on the board
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            // Check horizontal
            if (col + word.length() <= BOARD_SIZE) {
                std::string foundWord = "";
                for (size_t i = 0; i < word.length(); i++) {
                    foundWord += m_board[row][col + i];
                }
                if (foundWord == word) {
                    wordRow = row;
                    wordCol = col;
                    isHorizontal = true;
                    break;
                }
            }

            // Check vertical
            if (row + word.length() <= BOARD_SIZE) {
                std::string foundWord = "";
                for (size_t i = 0; i < word.length(); i++) {
                    foundWord += m_board[row + i][col];
                }
                if (foundWord == word) {
                    wordRow = row;
                    wordCol = col;
                    isHorizontal = false;
                    break;
                }
            }
        }
        if (wordRow != -1) break;
    }

    if (wordRow == -1) {
        // Word not found, return basic score
        int basicScore = 0;
        for (char c : word) {
            basicScore += m_tileValues[c];
        }
        return basicScore;
    }

    // Calculate score with multipliers
    return calculateWordScore(word, wordRow, wordCol, isHorizontal);
}

void ScrabbleGame::undoLastMove() {
    if (m_currentTurnTiles.empty()) {
        // No tiles to undo
        return;
    }

    // Get the last placed tile
    auto lastTile = m_currentTurnTiles.back();
    int row = lastTile.first;
    int col = lastTile.second;

    // Get the tile character
    char tileChar = m_board[row][col];

    // Remove tile from board
    m_board[row][col] = ' ';

    // Remove from current turn tiles
    m_currentTurnTiles.pop_back();

    // Add tile back to current player's rack
    if (m_currentPlayer == 1) {
        m_player1Tiles.push_back(tileChar);
    } else {
        m_player2Tiles.push_back(tileChar);
    }

    // Tile placement undone successfully
}

void ScrabbleGame::swapSelectedTile() {
    if (m_selectedTile < 0) {
        return; // No tile selected
    }

    if (m_hasSwappedThisTurn) {
        std::cout << "Already swapped this turn!" << std::endl;
        return; // Already swapped this turn
    }

    // Get current player's tiles
    std::vector<char>& currentTiles = (m_currentPlayer == 1) ? m_player1Tiles : m_player2Tiles;

    if (m_selectedTile >= static_cast<int>(currentTiles.size())) {
        return; // Invalid tile index
    }

    // Get the tile to swap
    char tileToSwap = currentTiles[m_selectedTile];

    // Add a new tile from the bag
    if (!m_tileBag.empty()) {
        char newTile = m_tileBag.back();
        m_tileBag.pop_back();

        // Replace the tile at the same position (swap in place)
        currentTiles[m_selectedTile] = newTile;

        std::cout << "Swapped '" << tileToSwap << "' for '" << newTile <<"'"<< std::endl;

        // Mark that we've swapped this turn
        m_hasSwappedThisTurn = true;
    } else {
        // If bag is empty, cannot swap
        std::cout << "Cannot swap - tile bag is empty!" << std::endl;
        return;
    }

    // Reset selection
    m_selectedTile = -1;
    m_isSwappingTile = false;
}

void ScrabbleGame::skipTurn() {
    if (m_currentTurnTiles.empty()) {
        // Player skipped turn (no tiles placed)
    } else {
        // Player skipped turn - returning tiles to rack

        // Return all placed tiles to the current player's rack
        for (const auto& tilePos : m_currentTurnTiles) {
            int row = tilePos.first;
            int col = tilePos.second;
            char tileChar = m_board[row][col];

            // Remove from board
            m_board[row][col] = ' ';

            // Add back to player's rack
            if (m_currentPlayer == 1) {
                m_player1Tiles.push_back(tileChar);
            } else {
                m_player2Tiles.push_back(tileChar);
            }
        }

        // Clear current turn tiles
        m_currentTurnTiles.clear();
    }

    // Switch to next player
    switchPlayer();
}
