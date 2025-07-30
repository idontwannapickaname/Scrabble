#include "../include/player.h"
#include <algorithm>

using namespace std;

Player::Player(const string &player_name) : name(player_name), score(0) {}

Player::~Player() {}

void Player::add_letter(char letter)
{
    if (rack.size() < 7)
    {
        rack.push_back(letter);
    }
}

void Player::remove_letters(const string &word)
{
    string temp_word = word;
    for (char c : word)
    {
        auto it = find(rack.begin(), rack.end(), c);
        if (it != rack.end())
        {
            rack.erase(it);
            temp_word.erase(temp_word.find(c), 1);
        }
    }
}

void Player::add_score(int points)
{
    score += points;
}

const vector<char> &Player::get_rack() const
{
    return rack;
}

void Player::set_rack(const vector<char> &new_rack)
{
    rack = new_rack;
}

int Player::get_score() const
{
    return score;
}

string Player::get_name() const
{
    return name;
}
