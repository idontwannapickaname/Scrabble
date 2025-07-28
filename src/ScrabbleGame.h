#ifndef SCRABBLEGAME_H
#define SCRABBLEGAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <map>

class ScrabbleGame {
private:
    SDL_Renderer* m_renderer;
    
    // Game board
    static const int BOARD_SIZE = 15;
    char m_board[BOARD_SIZE][BOARD_SIZE];
    int m_boardMultipliers[BOARD_SIZE][BOARD_SIZE]; // 1=normal, 2=double, 3=triple
    
    // Player tiles
    std::vector<char> m_playerTiles;
    std::vector<char> m_tileBag;
    
    // Game state
    int m_currentPlayer;
    int m_playerScores[2];
    bool m_gameRunning;
    
    // UI elements
    SDL_Rect m_boardRect;
    SDL_Rect m_tileRects[7];
    SDL_Rect m_scoreRect;
    SDL_Rect m_currentTileRect;
    
    // Colors
    SDL_Color m_boardColor;
    SDL_Color m_tileColor;
    SDL_Color m_textColor;
    SDL_Color m_selectedColor;
    
    // Fonts
    TTF_Font* m_tileFont;
    TTF_Font* m_boardTileFont;
    TTF_Font* m_scoreFont;
    
    // Selection
    int m_selectedTile;
    int m_selectedBoardX;
    int m_selectedBoardY;
    bool m_isPlacingTile;
    
    // Tile values
    std::map<char, int> m_tileValues;

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
    
    // Game logic
    void initializeBoard();
    void initializeTileBag();
    void drawPlayerTiles();
    void drawBoard();
    void drawScores();
    void placeTile(int boardX, int boardY, char tile);
    bool isValidPlacement(int x, int y);
    int calculateScore(const std::string& word);
    void refillPlayerTiles();
    
    // UI helpers
    void drawTile(int x, int y, char tile, bool selected = false);
    void drawText(int x, int y, const std::string& text, TTF_Font* font, SDL_Color color);
    
    bool isGameRunning() const { return m_gameRunning; }
};

#endif // SCRABBLEGAME_H 