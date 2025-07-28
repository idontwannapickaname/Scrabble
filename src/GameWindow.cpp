#include "GameWindow.h"
#include <iostream>

GameWindow::GameWindow() 
    : m_window(nullptr), m_renderer(nullptr), m_running(true), m_scrabbleGame(nullptr) {
    
    // Initialize colors
    m_backgroundColor = {240, 240, 240, 255}; // Light gray background
    m_textColor = {0, 0, 0, 255}; // Black text
    
    std::cout << "GameWindow initialized!" << std::endl;
}

GameWindow::~GameWindow() {
    if (m_scrabbleGame) {
        delete m_scrabbleGame;
    }
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

bool GameWindow::init() {
    // Create game window
    m_window = SDL_CreateWindow(
        "Scrabble Game", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        1024, 768, 
        SDL_WINDOW_SHOWN
    );
    
    if (!m_window) {
        std::cerr << "Failed to create game window: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create Scrabble game
    m_scrabbleGame = new ScrabbleGame(m_renderer);
    if (!m_scrabbleGame->init()) {
        std::cerr << "Failed to initialize Scrabble game!" << std::endl;
        return false;
    }
    
    std::cout << "Game window created successfully!" << std::endl;
    return true;
}

void GameWindow::run() {
    while (m_running && m_scrabbleGame->isGameRunning()) {
        handleInput();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void GameWindow::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Handle window-level events first
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;
        }
        
        // Pass all events to Scrabble game
        if (m_scrabbleGame) {
            m_scrabbleGame->handleEvent(event);
        }
    }
}

void GameWindow::update() {
    // Game logic updates can go here
}

void GameWindow::render() {
    // Let Scrabble game handle its own rendering
    if (m_scrabbleGame) {
        m_scrabbleGame->render();
    }
} 