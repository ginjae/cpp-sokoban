#pragma once
#include <vector>
#include <string>
#include <utility>

class solver {
public:
    std::vector<std::string> stage_data;
    std::pair<int, int> pos_player;
    std::vector<std::pair<int, int>> pos_storages;
    std::vector<std::pair<int, int>> pos_boxes;
    bool is_invalid = false;
    solver(std::vector<std::string> const& d);
    bool is_movable(int const row, int const col, std::pair<int, int> const& dir);
    int move(std::pair<int, int> const& dir);
    bool is_solved() const;
    bool is_stuck(int const row, int const col) const;
    bool is_deadlock() const;
    std::string get_string() const;
    std::string solve_bfs() const;
    int manhattan_sum() const;
    std::string solve_astar() const;
};

