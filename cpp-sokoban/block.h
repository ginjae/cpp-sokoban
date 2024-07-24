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
    virtual bool is_pushable(table const& tbl, block const& other) = 0;
    virtual char get_char() const = 0;
};

class player : public block {
public:
    player(int r, int c) : block(r, c) {}
    bool is_pushable(table const& tbl, block const& other) override { return false; }
    char get_char() const override { return PLAYER; }
    void move(table& tbl, std::pair<int, int> const& dir);
};

class wall : public block {
public:
    wall(int r, int c) : block(r, c) {}
    bool is_pushable(table const& tbl, block const& other) override { return false; }
    char get_char() const override { return WALL; }
};

class box : public block {
public:
    box(int r, int c) : block(r, c) {}
    bool is_pushable(table const& tbl, block const& other) override;
    char get_char() const override { return BOX; }
};
