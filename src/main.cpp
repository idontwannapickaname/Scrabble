#include "../include/GameLogic.h"
#include <iostream>

int main()
{
    GameLogic game(10, "../assets/dictionary/Collins Scrabble Words (2019).txt");

    while (true)
    {
        game.printBoard();

        std::string word;
        int row, col, dirInput;
        std::cout << "\nNhập từ (in hoa hoặc thường, ENTER để thoát): ";
        std::getline(std::cin, word);
        if (word.empty())
            break;

        std::cout << "Vị trí bắt đầu (row col): ";
        std::cin >> row >> col;

        std::cout << "Hướng (0: ngang, 1: dọc): ";
        std::cin >> dirInput;
        std::cin.ignore(); // clear newline

        Direction dir = (dirInput == 0) ? Direction::HORIZONTAL : Direction::VERTICAL;

        if (game.placeWord(word, {row, col}, dir))
        {
            int score = game.calculateScore(word, {row, col}, dir);
            std::cout << "✔️  Đặt từ thành công! Điểm: " << score << "\n";
        }
        else
        {
            std::cout << "❌ Không đặt được từ này.\n";
        }
    }

    std::cout << "Trò chơi kết thúc!\n";
    return 0;
}
