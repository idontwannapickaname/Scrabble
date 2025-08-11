#include "Menu.h"
#include <iostream>

Menu::Menu(SDL_Window* window, SDL_Renderer* renderer)
    : m_window(window), m_renderer(renderer), m_font(nullptr), m_titleFont(nullptr),
      m_selectedItem(-1), m_running(true), m_hoveredItem(-1) {

    // Initialize menu items
    m_menuItems = {"Play", "Quit"};

    // Initialize colors
    m_titleColor = {255, 215, 0, 255};      // Gold
    m_normalColor = {255, 255, 255, 255};   // White
    m_selectedColor = {255, 255, 0, 255};   // Yellow
    m_backgroundColor = {34, 139, 34, 255}; // Forest Green

    std::cout << "Menu initialized successfully!" << std::endl;
}

Menu::~Menu() {
    if (m_font) {
        TTF_CloseFont(m_font);
    }
    if (m_titleFont) {
        TTF_CloseFont(m_titleFont);
    }
}

bool Menu::init() {
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return false;
    }

    std::cout << "TTF_Init successful!" << std::endl;

    // Try to load fonts with multiple paths
    const char* fontPaths[] = {
        "arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
        "C:/Windows/Fonts/verdana.ttf",
        "C:/Windows/Fonts/msyh.ttc",  // Microsoft YaHei
        "C:/Windows/Fonts/simsun.ttc" // SimSun
    };

    // Load title font
    m_titleFont = nullptr;
    for (const char* path : fontPaths) {
        std::cout << "Trying to load title font from: " << path << std::endl;
        m_titleFont = TTF_OpenFont(path, 48);
        if (m_titleFont) {
            std::cout << "Title font loaded successfully from: " << path << std::endl;
            break;
        }
    }

    if (!m_titleFont) {
        std::cerr << "Failed to load title font. Trying default font..." << std::endl;
        m_titleFont = TTF_OpenFont("", 48); // Try default font
        if (!m_titleFont) {
            std::cerr << "Failed to load any title font: " << TTF_GetError() << std::endl;
            // Continue without font - we'll handle this in render
        }
    }

    // Load menu font
    m_font = nullptr;
    for (const char* path : fontPaths) {
        std::cout << "Trying to load menu font from: " << path << std::endl;
        m_font = TTF_OpenFont(path, 32);
        if (m_font) {
            std::cout << "Menu font loaded successfully from: " << path << std::endl;
            break;
        }
    }

    if (!m_font) {
        std::cerr << "Failed to load menu font. Trying default font..." << std::endl;
        m_font = TTF_OpenFont("", 32); // Try default font
        if (!m_font) {
            std::cerr << "Failed to load any menu font: " << TTF_GetError() << std::endl;
            // Continue without font - we'll handle this in render
        }
    }

    std::cout << "Menu initialization completed!" << std::endl;
    return true;
}

void Menu::run() {
    while (m_running) {
        handleInput();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Menu::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                m_running = false;
                break;

            case SDL_MOUSEMOTION:
                {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;

                    // Check if mouse is over any menu item
                    bool foundHover = false;
                    for (size_t i = 0; i < m_menuItems.size(); ++i) {
                        if (isMouseOverItem(mouseX, mouseY, i)) {
                            m_hoveredItem = i;
                            foundHover = true;
                            break;
                        }
                    }

                    if (!foundHover) {
                        m_hoveredItem = -1;
                    }
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;
                    handleMouseClick(mouseX, mouseY);
                }
                break;
        }
    }
}

void Menu::update() {
    // Update menu item rectangles for mouse interaction
    updateMenuRects();
}

void Menu::updateMenuRects() {
    m_menuItemRects.clear();

    int windowWidth, windowHeight;
    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

    int startY = 200;
    int itemSpacing = 60;
    int itemWidth = 200;
    int itemHeight = 50;

    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        SDL_Rect itemRect = {
            (windowWidth - itemWidth) / 2,
            startY + i * itemSpacing,
            itemWidth,
            itemHeight
        };
        m_menuItemRects.push_back(itemRect);
    }
}

bool Menu::isMouseOverItem(int x, int y, int itemIndex) {
    if (itemIndex < 0 || itemIndex >= static_cast<int>(m_menuItemRects.size())) {
        return false;
    }

    SDL_Rect& rect = m_menuItemRects[itemIndex];
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void Menu::handleMouseClick(int x, int y) {
    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        if (isMouseOverItem(x, y, i)) {
            m_selectedItem = i;
            m_running = false;
            break;
        }
    }
}

void Menu::render() {
    // Clear screen
    SDL_SetRenderDrawColor(m_renderer,
                          m_backgroundColor.r,
                          m_backgroundColor.g,
                          m_backgroundColor.b,
                          m_backgroundColor.a);
    SDL_RenderClear(m_renderer);

    int windowWidth, windowHeight;
    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

    // Render title
    if (m_titleFont) {
        SDL_Surface* titleSurface = TTF_RenderText_Solid(m_titleFont, "SCRABBLE", m_titleColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(m_renderer, titleSurface);
            if (titleTexture) {
                SDL_Rect titleRect = {
                    (windowWidth - titleSurface->w) / 2,
                    50,
                    titleSurface->w,
                    titleSurface->h
                };
                SDL_RenderCopy(m_renderer, titleTexture, nullptr, &titleRect);
                SDL_DestroyTexture(titleTexture);
            }
            SDL_FreeSurface(titleSurface);
        }
    } else {
        // Fallback: draw a simple rectangle for title
        SDL_Rect titleRect = {(windowWidth - 300) / 2, 50, 300, 60};
        SDL_SetRenderDrawColor(m_renderer, m_titleColor.r, m_titleColor.g, m_titleColor.b, m_titleColor.a);
        SDL_RenderFillRect(m_renderer, &titleRect);
    }

    // Render menu items
    int startY = 200;
    int itemSpacing = 60;

    for (size_t i = 0; i < m_menuItems.size(); ++i) {
        // Add selection indicator (mouse hover only)
        bool isHovered = (m_hoveredItem == i);

        if (isHovered) {
            SDL_Rect selectionRect = {
                (windowWidth - 200) / 2 - 20,
                startY + i * itemSpacing - 5,
                200 + 40,
                50
            };
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 100);
            SDL_RenderFillRect(m_renderer, &selectionRect);
        }

        if (m_font) {
            SDL_Color textColor;
            if (i == 0) { // "Play" item
                textColor = {0, 0, 0, 255}; // Black color for Play
            } else if (i == 1) { // "Quit" item
                textColor = {0, 0, 0, 255}; // Black color for Quit
            } else {
                textColor = (isHovered) ? m_selectedColor : m_normalColor;
            }
            SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, m_menuItems[i].c_str(), textColor);
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
                if (textTexture) {
                    SDL_Rect textRect = {
                        (windowWidth - textSurface->w) / 2,
                        startY + i * itemSpacing,
                        textSurface->w,
                        textSurface->h
                    };
                    SDL_RenderCopy(m_renderer, textTexture, nullptr, &textRect);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }
        } else {
            // Fallback: draw colored rectangles for menu items
            SDL_Rect itemRect = {(windowWidth - 150) / 2, startY + i * itemSpacing, 150, 40};
            SDL_Color itemColor;
            if (i == 0) { // "Play" item
                itemColor = {0, 0, 0, 255}; // Black color for Play
            } else if (i == 1) { // "Quit" item
                itemColor = {0, 0, 0, 255}; // Black color for Quit
            } else {
                itemColor = (isHovered) ? m_selectedColor : m_normalColor;
            }
            SDL_SetRenderDrawColor(m_renderer, itemColor.r, itemColor.g, itemColor.b, itemColor.a);
            SDL_RenderFillRect(m_renderer, &itemRect);
        }
    }

    // Render instructions
    if (m_font) {
        SDL_Color instructionColor = {200, 200, 200, 255};
        SDL_Surface* instructionSurface = TTF_RenderText_Solid(m_font,
            "Use Mouse Choose", instructionColor);
        if (instructionSurface) {
            SDL_Texture* instructionTexture = SDL_CreateTextureFromSurface(m_renderer, instructionSurface);
            if (instructionTexture) {
                SDL_Rect instructionRect = {
                    (windowWidth - instructionSurface->w) / 2,
                    windowHeight - 80,
                    instructionSurface->w,
                    instructionSurface->h
                };
                SDL_RenderCopy(m_renderer, instructionTexture, nullptr, &instructionRect);
                SDL_DestroyTexture(instructionTexture);
            }
            SDL_FreeSurface(instructionSurface);
        }
    } else {
        // Fallback: draw instruction rectangle
        SDL_Rect instructionRect = {(windowWidth - 400) / 2, windowHeight - 80, 400, 30};
        SDL_SetRenderDrawColor(m_renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(m_renderer, &instructionRect);
    }

    SDL_RenderPresent(m_renderer);
}

Menu::MenuAction Menu::getSelectedAction() const {
    switch (m_selectedItem) {
        case 0: return PLAY;
        case 1: return QUIT;
        default: return NONE;
    }
}
