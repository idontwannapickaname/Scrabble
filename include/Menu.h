#ifndef MENU_H
#define MENU_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class Menu {
private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;
    TTF_Font* m_titleFont;

    std::vector<std::string> m_menuItems;
    int m_selectedItem;
    bool m_running;

    // Colors
    SDL_Color m_titleColor;
    SDL_Color m_normalColor;
    SDL_Color m_selectedColor;
    SDL_Color m_backgroundColor;

    // Mouse interaction
    int m_hoveredItem;
    std::vector<SDL_Rect> m_menuItemRects;

public:
    Menu(SDL_Window* window, SDL_Renderer* renderer);
    ~Menu();

    bool init();
    void run();
    void render();
    void handleInput();
    void update();
    void updateMenuRects();
    bool isMouseOverItem(int x, int y, int itemIndex);
    void handleMouseClick(int x, int y);

    enum MenuAction {
        PLAY,
        QUIT,
        NONE
    };

    MenuAction getSelectedAction() const;
};

#endif // MENU_H
