#pragma once
#include <vector>
#include <string>
#include "block.h"

#define MAX_MAP_HEIGHT  16
#define MAX_MAP_WIDTH   16

class table {
private:
    int height, width;
    player* ptr_player;
    std::vector<std::pair<int, int>> storages;
public:
    std::vector<std::vector<block *>> map;
    bool invalid_flag = false;
    bool solved_flag = false;
    table(int const h, int const w, std::vector<std::string> const& data);
    int get_height() const { return this->height; }
    int get_width() const { return this->width; }
    void draw_table() const;
    player* get_player() const { return this->ptr_player; }
    void set_player(player* p) { this->ptr_player = p; }
    bool has_storage_at(int const r, int const c) const;
    bool is_solved() const;
    ~table();   // needs Rule of 3?
};

