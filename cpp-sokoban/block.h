#pragma once
#include <utility>

class table;

constexpr char PLAYER   = '@';
constexpr char WALL     = '#';
constexpr char BOX      = '$';
constexpr char STORAGE  = '.';
constexpr char PLAYER_S = '+';
constexpr char BOX_S    = '*';
constexpr char FREE     = ' ';


class block {
protected:
    int row, col;
public:
    block(int r, int c) : row(r), col(c) {}
    virtual ~block() {}
    void set_row(int r) { this->row = r; }
    void set_col(int c) { this->col = c; }
    int get_row() const { return this->row; }
    int get_col() const { return this->col; }
    virtual void move(table& tbl, std::pair<int, int> const& dir, bool update_flag = true) = 0;
    virtual bool is_movable(table& tbl, std::pair<int, int> const& dir) const = 0;
    virtual char get_char() const = 0;
    virtual block* copy() const = 0;
};

class player : public block {
public:
    player(int r, int c) : block(r, c) {}
    void move(table& tbl, std::pair<int, int> const& dir, bool update_flag = true) override;
    bool is_movable(table& tbl, std::pair<int, int> const& dir) const override;
    char get_char() const override { return PLAYER; }
    block* copy() const override { return new player(this->row, this->col); }
};

class wall : public block {
public:
    wall(int r, int c) : block(r, c) {}
    void move(table& tbl, std::pair<int, int> const& dir, bool update_flag = true) override {}
    bool is_movable(table& tbl, std::pair<int, int> const& dir) const override { return false; }
    char get_char() const override { return WALL; }
    block* copy() const override { return new wall(this->row, this->col); }
};

class box : public block {
public:
    box(int r, int c) : block(r, c) {}
    void move(table& tbl, std::pair<int, int> const& dir, bool update_flag = true) override;
    bool is_movable(table& tbl, std::pair<int, int> const& dir) const override;
    char get_char() const override { return BOX; }
    block* copy() const override { return new box(this->row, this->col); }
};
