#ifndef SCRABBLEGAME_H
#define SCRABBLEGAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <map>
#include <set>

class ScrabbleGame {
private:
    SDL_Renderer* m_renderer;

    // Game board
    static const int BOARD_SIZE = 15;
    char m_board[BOARD_SIZE][BOARD_SIZE];
    int m_boardMultipliers[BOARD_SIZE][BOARD_SIZE]; // 1=normal, 2=double, 3=triple
    bool m_boardWordMultipliers[BOARD_SIZE][BOARD_SIZE];  // true=word multiplier, false=letter multiplier

    // Player tiles
    std::vector<char> m_player1Tiles;  // Player 1's tiles
    std::vector<char> m_player2Tiles;  // Player 2's tiles
    std::vector<char> m_tileBag;

    // Game state
    int m_player1Score;  // Player 1 score
    int m_player2Score;  // Player 2 score
    int m_currentPlayer; // 1 for player 1, 2 for player 2
    bool m_gameRunning;

    // Dictionary
    std::set<std::string> m_dictionary;

    // Track current turn tiles
    std::vector<std::pair<int, int>> m_currentTurnTiles; // Track positions of tiles placed this turn

    // Error message for invalid words
    std::string m_errorMessage;
    bool m_showError;

    // UI elements
    SDL_Rect m_boardRect;
    SDL_Rect m_tileRects[7];
    SDL_Rect m_scoreRect;
    SDL_Rect m_player1Rect;
    SDL_Rect m_player2Rect;
    SDL_Rect m_currentTileRect;
    SDL_Rect m_quitButtonRect; // Add quit button
    SDL_Rect m_submitButtonRect; // Add submit button
    SDL_Rect m_undoButtonRect; // Add undo button
    SDL_Rect m_skipButtonRect; // Add skip button
    SDL_Rect m_swapButtonRect; // Add swap button
    SDL_Rect m_errorLabelRect; // Add error label area
    SDL_Rect m_quitButton;      // Quit button for click detection
    SDL_Rect m_submitButton;    // Submit button for click detection
    SDL_Rect m_undoButton;      // Undo button for click detection
    SDL_Rect m_skipButton;      // Skip button for click detection
    SDL_Rect m_swapButton;      // Swap button for click detection

    // Colors
    SDL_Color m_boardColor;
    SDL_Color m_tileColor;
    SDL_Color m_textColor;
    SDL_Color m_selectedColor;
    SDL_Color m_quitButtonColor; // Add quit button color
    SDL_Color m_submitButtonColor; // Add submit button color
    SDL_Color m_player1Color;    // Player 1 color
    SDL_Color m_player2Color;    // Player 2 color

    // Button hover states
    bool m_quitButtonHovered;
    bool m_submitButtonHovered;
    bool m_undoButtonHovered;
    bool m_skipButtonHovered;
    bool m_swapButtonHovered;

    // Fonts
    TTF_Font* m_tileFont;
    TTF_Font* m_boardTileFont;
    TTF_Font* m_scoreFont;

    // Selection
    int m_selectedTile;
    int m_selectedBoardX;
    int m_selectedBoardY;
    bool m_isPlacingTile;
    bool m_isSwappingTile; // Track if player is in swap mode
    bool m_hasSwappedThisTurn; // Track if player has already swapped this turn

    // Tile values
    std::map<char, int> m_tileValues;

    // Helper methods
    void loadDictionary();
    bool isValidWord(const std::string& word);
    std::vector<std::string> findWordsOnBoard();
    std::vector<std::string> findWordsInDirection(int startRow, int startCol, bool horizontal);
    int calculateWordScore(const std::string& word, int row, int col, bool horizontal);
    bool isValidPlacement(int x, int y, bool horizontal, const std::string& word);
    std::vector<std::string> findNewWords();
    int calculateWordScoreWithPosition(const std::string& word);
    std::string findWordAtPosition(int row, int col, bool horizontal);
    bool containsNewTile(const std::string& word, int row, int col, bool horizontal);

public:
    ScrabbleGame(SDL_Renderer* renderer);
    ~ScrabbleGame();

    bool init();
    void run();
    void render();
    void handleInput();
    void update();

    // Add method to handle events from GameWindow
    void handleEvent(const SDL_Event& event);

    // Game state
    bool isRunning() const { return m_gameRunning; }

    // Game logic
    void initializeBoard();
    void initializeTileBag();
    void drawPlayerTiles();
    void refillCurrentPlayerTiles();
    void drawBoard();
    void drawScores();
    void drawPlayerLabels();
    void placeTile(int boardX, int boardY, char tile);
    bool isValidPlacement(int x, int y);
    int calculateScore(const std::string& word);
    void refillPlayerTiles();
    void resetGame(); // Add new method to reset game with new tiles
    void drawQuitButton(); // Add method to draw quit button
    bool isQuitButtonClicked(int x, int y); // Add method to check quit button click
    void drawSubmitButton(); // Add method to draw submit button
    bool isSubmitButtonClicked(int x, int y); // Add method to check submit button click
    void switchPlayer(); // Add method to switch player turn
    bool submitTurn(); // New method to submit and validate turn
    void undoLastMove();     // New undo method
    void skipTurn();         // New skip method

    // UI helpers
    void drawTile(int x, int y, char tile, bool selected = false);
    void drawText(int x, int y, const std::string& text, TTF_Font* font,
                  SDL_Color color);
    void drawUndoButton();       // New undo button drawing
    void drawSkipButton();       // New skip button drawing
    bool isUndoButtonClicked(int x, int y);    // New undo button click detection
    bool isSkipButtonClicked(int x, int y);    // New skip button click detection
    void drawSwapButton();       // New swap button drawing
    bool isSwapButtonClicked(int x, int y);    // New swap button click detection
    void swapSelectedTile();     // New swap tile method
    void drawErrorLabel();       // New error label drawing
    void setErrorMessage(const std::string& message); // Set error message
};

#endif
