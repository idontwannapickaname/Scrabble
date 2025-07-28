#include "../include/GameLogic.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include <iomanip>

GameLogic::GameLogic(int boardSize, const std::string &dictionaryPath)
    : board(boardSize, std::vector<Tile>(boardSize))
{
    initLetterValues();
    loadDictionary(dictionaryPath);
}

void GameLogic::initLetterValues()
{
    letterValues = {
        {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2}, {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1}, {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1}, {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10}};
}

void GameLogic::loadDictionary(const std::string &path)
{
    std::ifstream file(path);
    std::string word;
    while (std::getline(file, word))
    {
        if (!word.empty())
        {
            for (char &c : word)
                c = std::toupper(c);
            dictionary.insert(word);
        }
    }
}

bool GameLogic::isWordValid(const std::string &word)
{
    std::string upper = word;
    for (char &c : upper)
        c = std::toupper(c);
    return dictionary.count(upper);
}

bool GameLogic::placeWord(const std::string &word, Position pos, Direction dir)
{
    if (!isWordValid(word))
        return false;

    int r = pos.row, c = pos.col;
    for (char ch : word)
    {
        if (r >= board.size() || c >= board[0].size())
            return false;
        board[r][c] = {(char)std::toupper(ch), letterValues[ch], true};
        if (dir == Direction::HORIZONTAL)
            ++c;
        else
            ++r;
    }
    return true;
}

int GameLogic::calculateScore(const std::string &word, Position pos, Direction dir)
{
    int score = 0;
    int r = pos.row, c = pos.col;
    for (char ch : word)
    {
        score += letterValues[ch];
        if (dir == Direction::HORIZONTAL)
            ++c;
        else
            ++r;
    }
    return score;
}

void GameLogic::printBoard()
{
    std::cout << "   ";
    for (int c = 0; c < board[0].size(); ++c)
        std::cout << std::setw(2) << c;
    std::cout << "\n";

    for (int r = 0; r < board.size(); ++r)
    {
        std::cout << std::setw(2) << r << " ";
        for (int c = 0; c < board[r].size(); ++c)
        {
            char ch = board[r][c].isPlaced ? board[r][c].letter : '.';
            std::cout << ch << " ";
        }
        std::cout << "\n";
    }
}
