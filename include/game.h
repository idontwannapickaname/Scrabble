#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <set>
#include <random>
#include "player.h"

using namespace std;

class Game
{
private:
    static const int BOARD_SIZE = 15;     // Kích thước bàn cờ 15x15
    vector<vector<char>> board; // Bàn cờ
    vector<char> letter_bag;         // Túi chữ cái
    set<string> dictionary;     // Từ điển
    vector<Player> players;          // Danh sách người chơi
    int current_player;                   // Người chơi hiện tại
    mt19937 rng;                     // Random number generator

    // Khởi tạo túi chữ cái theo phân phối của Scrabble
    void initialize_letter_bag();
    // Nạp từ điển từ file
    void load_dictionary(const string &filename);
    // Kiểm tra xem từ có hợp lệ trong từ điển không
    bool is_valid_word(const string &word);
    // Tính điểm cho một từ được đặt
    int calculate_word_score(const string &word, int row, int col, bool horizontal);
    // Kiểm tra các từ phụ được tạo ra
    vector<string> check_additional_words(const string &word, int row, int col, bool horizontal);
    // Kiểm tra xem từ có đặt hợp lệ trên bàn cờ không
    bool is_valid_placement(const string &word, int row, int col, bool horizontal);

public:
    Game(int num_players);
    ~Game();

    // Bắt đầu trò chơi
    void start_game();
    // Đặt từ lên bàn cờ
    bool place_word(const string &word, int row, int col, bool horizontal);
    // Đổi chữ cái
    bool swap_letters(const vector<char> &letters);
    // Bỏ lượt
    void pass_turn();
    // Lấy chữ cái ngẫu nhiên từ túi
    char draw_letter();
    // Kết thúc lượt của người chơi hiện tại
    void end_turn();
    // Lấy người chơi hiện tại
    Player &get_current_player();
    // Lấy bàn cờ
    const vector<vector<char>> &get_board() const;
    // Kiểm tra xem trò chơi đã kết thúc chưa
    bool is_game_over();
    // Lấy người chiến thắng
    Player *get_winner();
};

#endif // GAME_H