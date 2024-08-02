#pragma once
#include <vector>
#include <string>
#include <utility>

class solver {
public:
    std::vector<std::string> stage_data;
    std::pair<int, int> pos_player = { -1, -1 };
    std::vector<std::pair<int, int>> pos_storages;
    bool is_invalid = false;
    solver(std::vector<std::string> const &d);
    solver(solver const& other);
    solver& operator=(solver const& other);
    bool is_pushable(int const row, int const col, std::pair<int, int> const& dir) const;
    bool is_movable(std::pair<int, int> const& dir);
    void move(std::pair<int, int> const& dir);
    bool is_solved() const;
    bool is_stuck() const;
    std::string get_string() const;
    std::string solve_bfs() const;
};

