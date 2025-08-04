#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <SDL2/SDL.h>
#include <string>
#include "ScrabbleGame.h"

class GameWindow {
private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    bool m_running;
    ScrabbleGame* m_scrabbleGame;
    
    // Colors
    SDL_Color m_backgroundColor;
    SDL_Color m_textColor;

public:
    GameWindow();
    ~GameWindow();
    
    bool init();
    void run();
    void render();
    void handleInput();
    void update();
    
    bool isRunning() const { return m_running; }
    bool isGameRunning() const; // Add method to check if Scrabble game is still running
};

#endif // GAMEWINDOW_H 