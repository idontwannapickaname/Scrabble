#include "../include/game.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

// Phân phối chữ cái và điểm số theo chuẩn Scrabble (tiếng Anh)
const vector<pair<char, int>> LETTER_DISTRIBUTION = {
    {'A', 1}, {'A', 1}, {'A', 1}, {'A', 1}, {'A', 1}, {'A', 1}, {'A', 1}, {'A', 1}, {'A', 1}, {'B', 3}, {'B', 3}, {'C', 3}, {'C', 3}, {'D', 2}, {'D', 2}, {'D', 2}, {'D', 2}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'E', 1}, {'F', 4}, {'F', 4}, {'G', 2}, {'G', 2}, {'G', 2}, {'H', 4}, {'H', 4}, {'I', 1}, {'I', 1}, {'I', 1}, {'I', 1}, {'I', 1}, {'I', 1}, {'I', 1}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'L', 1}, {'L', 1}, {'L', 1}, {'M', 3}, {'M', 3}, {'N', 1}, {'N', 1}, {'N', 1}, {'N', 1}, {'N', 1}, {'N', 1}, {'O', 1}, {'O', 1}, {'O', 1}, {'O', 1}, {'O', 1}, {'O', 1}, {'O', 1}, {'O', 1}, {'P', 3}, {'P', 3}, {'Q', 10}, {'R', 1}, {'R', 1}, {'R', 1}, {'R', 1}, {'R', 1}, {'R', 1}, {'S', 1}, {'S', 1}, {'S', 1}, {'S', 1}, {'T', 1}, {'T', 1}, {'T', 1}, {'T', 1}, {'T', 1}, {'T', 1}, {'U', 1}, {'U', 1}, {'U', 1}, {'U', 1}, {'V', 4}, {'V', 4}, {'W', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Y', 4}, {'Z', 10}};

// Bố cục ô đặc biệt (Double Letter, Triple Letter, Double Word, Triple Word)
const vector<vector<string>> SPECIAL_TILES = {
    {"TW", ".", ".", "DL", ".", ".", ".", "TW", ".", ".", ".", "DL", ".", ".", "TW"},
    {".", "DW", ".", ".", ".", "TL", ".", ".", ".", "TL", ".", ".", ".", "DW", "."},
    {".", ".", "DW", ".", ".", ".", "DL", ".", "DL", ".", ".", ".", "DW", ".", "."},
    {"DL", ".", ".", "DW", ".", ".", ".", "DL", ".", ".", ".", "DW", ".", ".", "DL"},
    {".", ".", ".", ".", "DW", ".", ".", ".", ".", ".", "DW", ".", ".", ".", "."},
    {".", "TL", ".", ".", ".", "TL", ".", ".", ".", "TL", ".", ".", ".", "TL", "."},
    {".", ".", "DL", ".", ".", ".", "DL", ".", "DL", ".", ".", ".", "DL", ".", "."},
    {"TW", ".", ".", "DL", ".", ".", ".", "DW", ".", ".", ".", "DL", ".", ".", "TW"},
    {".", ".", "DL", ".", ".", ".", "DL", ".", "DL", ".", ".", ".", "DL", ".", "."},
    {".", "TL", ".", ".", ".", "TL", ".", ".", ".", "TL", ".", ".", ".", "TL", "."},
    {".", ".", ".", ".", "DW", ".", ".", ".", ".", ".", "DW", ".", ".", ".", "."},
    {"DL", ".", ".", "DW", ".", ".", ".", "DL", ".", ".", ".", "DW", ".", ".", "DL"},
    {".", ".", "DW", ".", ".", ".", "DL", ".", "DL", ".", ".", ".", "DW", ".", "."},
    {".", "DW", ".", ".", ".", "TL", ".", ".", ".", "TL", ".", ".", ".", "DW", "."},
    {"TW", ".", ".", "DL", ".", ".", ".", "TW", ".", ".", ".", "DL", ".", ".", "TW"}};

Game::Game(int num_players) : current_player(0), rng(time(nullptr))
{
    // Khởi tạo bàn cờ rỗng
    board.resize(BOARD_SIZE, vector<char>(BOARD_SIZE, '.'));

    // Khởi tạo túi chữ cái
    initialize_letter_bag();

    // Nạp từ điển
    load_dictionary("/home/minhbui/Code/Scrabble/assets/dictionary/dictionary.txt");

    // Khởi tạo người chơi
    for (int i = 0; i < num_players; ++i)
    {
        players.emplace_back("Player " + to_string(i + 1));
        // Bốc 7 chữ cái cho mỗi người chơi
        for (int j = 0; j < 7; ++j)
        {
            if (!letter_bag.empty())
            {
                players[i].add_letter(draw_letter());
            }
        }
    }
}

Game::~Game() {}

void Game::load_dictionary(const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Cannot open dictionary file " << filename << endl;
        dictionary = {"HELLO", "WORLD", "SCRABBLE", "GAME"}; // Từ điển mặc định
        return;
    }

    string word;
    int loaded_words = 0;
    while (getline(file, word))
    {
        // Trim khoảng trắng đầu và cuối (nếu có)
        word.erase(word.begin(), find_if(word.begin(), word.end(), [](unsigned char ch)
                                         { return !isspace(ch); }));
        word.erase(find_if(word.rbegin(), word.rend(), [](unsigned char ch)
                           { return !isspace(ch); })
                       .base(),
                   word.end());

        // Chuyển thành chữ hoa
        transform(word.begin(), word.end(), word.begin(), ::toupper);

        // Kiểm tra từ có hợp lệ không (chỉ A-Z)
        if (!word.empty() && word.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") == string::npos)
        {
            dictionary.insert(word);
            loaded_words++;
        }
        else if (!word.empty())
        {
            cerr << "Skipping invalid word: " << word << endl;
        }
    }

    file.close();
    // cout << "Dictionary loaded with " << loaded_words << " valid words." << endl;
}

void Game::initialize_letter_bag()
{
    letter_bag.clear();
    for (const auto &pair : LETTER_DISTRIBUTION)
    {
        letter_bag.push_back(pair.first);
    }
    shuffle(letter_bag.begin(), letter_bag.end(), rng);
}

bool Game::is_valid_word(const string &word)
{
    return dictionary.count(word) > 0;
}

int Game::calculate_word_score(const string &word, int row, int col, bool horizontal)
{
    int score = 0;
    int word_multiplier = 1;

    // Tính điểm cho từ chính
    for (size_t i = 0; i < word.length(); ++i)
    {
        int r = row, c = col;
        if (horizontal)
            c += i;
        else
            r += i;

        int letter_score = 0;
        for (const auto &pair : LETTER_DISTRIBUTION)
        {
            if (pair.first == word[i])
            {
                letter_score = pair.second;
                break;
            }
        }

        // Áp dụng nhân điểm cho ô đặc biệt
        if (board[r][c] == '.')
        { // Chỉ áp dụng nếu ô chưa có chữ
            if (SPECIAL_TILES[r][c] == "DL")
                letter_score *= 2;
            else if (SPECIAL_TILES[r][c] == "TL")
                letter_score *= 3;
            else if (SPECIAL_TILES[r][c] == "DW")
                word_multiplier *= 2;
            else if (SPECIAL_TILES[r][c] == "TW")
                word_multiplier *= 3;
        }
        score += letter_score;
    }
    score *= word_multiplier;

    // Kiểm tra và tính điểm cho từ phụ
    vector<string> additional_words = check_additional_words(word, row, col, horizontal);
    for (const auto &add_word : additional_words)
    {
        int add_score = 0;
        int add_word_multiplier = 1;
        for (size_t i = 0; i < add_word.length(); ++i)
        {
            int r = row, c = col;
            if (horizontal)
                r += i;
            else
                c += i;

            int letter_score = 0;
            for (const auto &pair : LETTER_DISTRIBUTION)
            {
                if (pair.first == add_word[i])
                {
                    letter_score = pair.second;
                    break;
                }
            }

            if (board[r][c] == '.')
            { // Chỉ áp dụng nếu ô chưa có chữ
                if (SPECIAL_TILES[r][c] == "DL")
                    letter_score *= 2;
                else if (SPECIAL_TILES[r][c] == "TL")
                    letter_score *= 3;
                else if (SPECIAL_TILES[r][c] == "DW")
                    add_word_multiplier *= 2;
                else if (SPECIAL_TILES[r][c] == "TW")
                    add_word_multiplier *= 3;
            }
            add_score += letter_score;
        }
        score += add_score * add_word_multiplier;
    }

    // Thưởng 50 điểm nếu dùng hết 7 chữ cái
    if (word.length() == 7)
        score += 50;

    return score;
}

vector<string> Game::check_additional_words(const string &word, int row, int col, bool horizontal)
{
    vector<string> additional_words;

    // Kiểm tra từ phụ theo hướng vuông góc
    for (size_t i = 0; i < word.length(); ++i)
    {
        int r = row, c = col;
        if (horizontal)
            c += i;
        else
            r += i;

        // Chỉ kiểm tra nếu ô được đặt chữ mới
        if (board[r][c] != '.')
            continue;

        string add_word;
        int start_r = r, start_c = c;

        // Tìm từ theo hướng vuông góc
        if (horizontal)
        {
            // Tìm lên
            while (start_r > 0 && board[start_r - 1][c] != '.')
            {
                --start_r;
            }
            // Tìm xuống
            while (start_r < BOARD_SIZE && (board[start_r][c] != '.' || (start_r == r && word[i] != '\0')))
            {
                if (start_r == r)
                    add_word += word[i];
                else
                    add_word += board[start_r][c];
                ++start_r;
            }
        }
        else
        {
            // Tìm trái
            while (start_c > 0 && board[r][start_c - 1] != '.')
            {
                --start_c;
            }
            // Tìm phải
            while (start_c < BOARD_SIZE && (board[r][start_c] != '.' || (start_c == c && word[i] != '\0')))
            {
                if (start_c == c)
                    add_word += word[i];
                else
                    add_word += board[r][start_c];
                ++start_c;
            }
        }

        if (add_word.length() > 1 && is_valid_word(add_word))
        {
            additional_words.push_back(add_word);
        }
    }

    return additional_words;
}

bool Game::is_valid_placement(const string &word, int row, int col, bool horizontal)
{
    // Kiểm tra từ có hợp lệ trong từ điển
    if (!is_valid_word(word))
        return false;

    // Kiểm tra vượt quá bàn cờ
    if (horizontal)
    {
        if (col + word.length() > BOARD_SIZE)
            return false;
    }
    else
    {
        if (row + word.length() > BOARD_SIZE)
            return false;
    }

    // Kiểm tra giá chữ của người chơi
    vector<char> rack = players[current_player].get_rack();
    string temp_word = word;
    for (char c : word)
    {
        auto it = find(rack.begin(), rack.end(), c);
        if (it == rack.end())
        {
            // Kiểm tra xem ô trên bàn cờ đã có chữ cái này chưa
            int r = row, c = col;
            if (horizontal)
                c += (temp_word.find(c) - temp_word.find(temp_word[0]));
            else
                r += (temp_word.find(c) - temp_word.find(temp_word[0]));
            if (board[r][c] != c)
                return false;
        }
        else
        {
            rack.erase(it);
        }
    }

    // Kiểm tra kết nối với từ hiện có (bỏ qua nếu bàn cờ rỗng)
    bool connected = false;
    if (all_of(board.begin(), board.end(), [](const vector<char> &row)
               { return all_of(row.begin(), row.end(), [](char c)
                               { return c == '.'; }); }))
    {
        // Lượt đầu tiên, từ phải qua ô trung tâm (7,7)
        if (horizontal && row != 7)
            return false;
        if (!horizontal && col != 7)
            return false;
        if (horizontal && (col > 7 || col + word.length() <= 7))
            return false;
        if (!horizontal && (row > 7 || row + word.length() <= 7))
            return false;
        connected = true;
    }
    else
    {
        // Kiểm tra kết nối với từ hiện có
        for (size_t i = 0; i < word.length(); ++i)
        {
            int r = row, c = col;
            if (horizontal)
                c += i;
            else
                r += i;
            if (r > 0 && board[r - 1][c] != '.')
                connected = true;
            if (r < BOARD_SIZE - 1 && board[r + 1][c] != '.')
                connected = true;
            if (c > 0 && board[r][c - 1] != '.')
                connected = true;
            if (c < BOARD_SIZE - 1 && board[r][c + 1] != '.')
                connected = true;
        }
    }

    // Kiểm tra từ phụ
    vector<string> additional_words = check_additional_words(word, row, col, horizontal);
    for (const auto &add_word : additional_words)
    {
        if (!is_valid_word(add_word))
            return false;
    }

    return connected;
}

bool Game::place_word(const string &word, int row, int col, bool horizontal)
{
    if (!is_valid_placement(word, row, col, horizontal))
        return false;

    // Đặt từ lên bàn cờ
    for (size_t i = 0; i < word.length(); ++i)
    {
        int r = row, c = col;
        if (horizontal)
            c += i;
        else
            r += i;
        if (board[r][c] == '.')
        {
            board[r][c] = word[i];
        }
    }

    // Cập nhật điểm số và giá chữ
    int score = calculate_word_score(word, row, col, horizontal);
    players[current_player].add_score(score);
    players[current_player].remove_letters(word);

    // Bốc chữ cái mới
    for (size_t i = 0; i < word.length() && !letter_bag.empty(); ++i)
    {
        if (board[row][col] == '.')
        { // Chỉ bốc nếu ô chưa có chữ
            players[current_player].add_letter(draw_letter());
        }
        if (horizontal)
            ++col;
        else
            ++row;
    }

    return true;
}

char Game::draw_letter()
{
    if (letter_bag.empty())
        return '\0';
    char letter = letter_bag.back();
    letter_bag.pop_back();
    return letter;
}

void Game::end_turn()
{
    current_player = (current_player + 1) % players.size();
}

bool Game::swap_letters(const vector<char> &letters)
{
    if (letters.empty() || letter_bag.size() < letters.size())
        return false;

    vector<char> rack = players[current_player].get_rack();
    for (char c : letters)
    {
        auto it = find(rack.begin(), rack.end(), c);
        if (it == rack.end())
            return false;
        rack.erase(it);
        letter_bag.push_back(c);
    }

    shuffle(letter_bag.begin(), letter_bag.end(), rng);

    for (size_t i = 0; i < letters.size() && !letter_bag.empty(); ++i)
    {
        rack.push_back(draw_letter());
    }

    players[current_player].set_rack(rack);
    return true;
}

void Game::pass_turn()
{
    end_turn();
}

Player &Game::get_current_player()
{
    return players[current_player];
}

const vector<vector<char>> &Game::get_board() const
{
    return board;
}

bool Game::is_game_over()
{
    return letter_bag.empty() && any_of(players.begin(), players.end(),
                                        [](const Player &p)
                                        { return p.get_rack().empty(); });
}

Player *Game::get_winner()
{
    if (!is_game_over())
        return nullptr;
    return &*max_element(players.begin(), players.end(),
                         [](const Player &a, const Player &b)
                         { return a.get_score() < b.get_score(); });
}
