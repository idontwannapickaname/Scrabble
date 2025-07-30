#include "../include/game.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

// In bàn cờ
void print_board(const vector<vector<char>> &board)
{
    cout << "   ";
    for (int i = 0; i < 15; ++i)
        cout << i % 10 << " ";
    cout << endl;
    for (int i = 0; i < 15; ++i)
    {
        cout << i % 10 << " ";
        for (int j = 0; j < 15; ++j)
        {
            cout << (board[i][j] == '.' ? '.' : board[i][j]) << " ";
        }
        cout << endl;
    }
}

// In giá chữ
void print_rack(const vector<char> &rack)
{
    cout << "Rack: ";
    for (char c : rack)
        cout << c << " ";
    cout << endl;
}

int main()
{
    cout << "Starting Scrabble Game (2 players)\n";
    Game game(2);

    while (!game.is_game_over())
    {
        Player &player = game.get_current_player();
        cout << "\n=== " << player.get_name() << "'s Turn ===\n";
        cout << "Score: " << player.get_score() << endl;
        print_rack(player.get_rack());
        print_board(game.get_board()); // Sửa lỗi: gọi get_board thay vì get_rack

        cout << "\nOptions:\n";
        cout << "1. Place word (e.g., 'word row col h/v')\n";
        cout << "2. Swap letters (e.g., 'A B C')\n";
        cout << "3. Pass turn\n";
        cout << "Enter choice (1/2/3): ";

        int choice;
        cin >> choice;
        cin.ignore(); // Xóa bộ đệm

        if (choice == 1)
        {
            string input;
            cout << "Enter word, row, col, direction (h/v): ";
            getline(cin, input);
            stringstream ss(input);
            string word;
            int row, col;
            char dir;
            ss >> word >> row >> col >> dir;

            // Chuyển từ thành chữ hoa
            transform(word.begin(), word.end(), word.begin(), ::toupper);

            bool horizontal = (dir == 'h' || dir == 'H');
            if (game.place_word(word, row, col, horizontal))
            {
                cout << "Word placed successfully! Score: " << player.get_score() << endl;
                game.end_turn();
            }
            else
            {
                cout << "Invalid placement. Try again.\n";
            }
        }
        else if (choice == 2)
        {
            string input;
            cout << "Enter letters to swap (e.g., A B C): ";
            getline(cin, input);
            stringstream ss(input);
            vector<char> letters;
            char c;
            while (ss >> c)
            {
                letters.push_back(toupper(c));
            }

            if (game.swap_letters(letters))
            {
                cout << "Letters swapped successfully!\n";
                game.end_turn();
            }
            else
            {
                cout << "Invalid swap. Try again.\n";
            }
        }
        else if (choice == 3)
        {
            game.pass_turn();
            cout << "Turn passed.\n";
        }
        else
        {
            cout << "Invalid choice. Try again.\n";
        }
    }

    Player *winner = game.get_winner();
    if (winner)
    {
        cout << "\nGame Over! Winner: " << winner->get_name() << " with score " << winner->get_score() << endl;
    }
    else
    {
        cout << "\nGame Over! No winner determined.\n";
    }

    return 0;
}
