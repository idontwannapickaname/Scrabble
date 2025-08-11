#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>

using namespace std;

class Player
{
private:
    string name;       // Tên người chơi
    vector<char> rack; // Giá chữ (tối đa 7 chữ cái)
    int score;              // Điểm số

public:
    Player(const string &player_name);
    ~Player();

    // Thêm chữ cái vào giá
    void add_letter(char letter);
    // Xóa chữ cái khỏi giá
    void remove_letters(const string &word);
    // Cập nhật điểm số
    void add_score(int points);
    // Lấy giá chữ hiện tại
    const vector<char> &get_rack() const;
    // Thiết lập giá chữ mới
    void set_rack(const vector<char> &new_rack);
    // Lấy điểm số
    int get_score() const;
    // Lấy tên người chơi
    string get_name() const;
};

#endif // PLAYER_H