#pragma once
#include <vector>
#include <string>
#include "block.h"

#define MAX_MAP_HEIGHT  32
#define MAX_MAP_WIDTH   32

class table {
private:
    int height, width;
    player* ptr_player;
    std::vector<std::pair<int, int>> storages;
    std::vector<std::vector<block *>> map;
public:
    bool is_invalid = false;
    table(int const h, int const w, std::vector<std::string> const& data);
    int get_height() const { return this->height; }
    int get_width() const { return this->width; }
    void draw_table() const;
    player* get_player() { return this->ptr_player; }
    void set_player(player* p) { this->ptr_player = p; }
    block* get_block(int const r, int const c) { return this->map[r][c]; }
    void set_block(int const r, int const c, block* b) { this->map[r][c] = b; }
    bool has_storage_at(int const r, int const c) const;
    bool is_solved() const;
    ~table();   // needs Rule of 3?
};

