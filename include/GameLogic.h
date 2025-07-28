#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

struct Tile {
    char letter = ' ';
    int value = 0;
    bool isPlaced = false;
};

struct Position {
    int row;
    int col;
};

enum class Direction {
    HORIZONTAL,
    VERTICAL
};

class GameLogic {
public:
    GameLogic(int boardSize, const std::string& dictionaryPath);

    bool placeWord(const std::string& word, Position pos, Direction dir);
    bool isWordValid(const std::string& word);
    int calculateScore(const std::string& word, Position pos, Direction dir);
    void printBoard();

private:
    std::vector<std::vector<Tile>> board;
    std::set<std::string> dictionary;
    std::unordered_map<char, int> letterValues;

    void initLetterValues();
    void loadDictionary(const std::string& path);
};

#endif
