#include "solver.h"
#include <algorithm>
#include <queue>
#include <unordered_set>

using namespace std;

extern pair<int, int> dirs[4];
char dir_to_char(pair<int, int> const& dir);

solver::solver(vector<string> const& d) : stage_data(d), pos_player({ -1, -1 }) {
    if (d.size() > 16 || d[0].size() > 16)
        is_invalid = true;
    for (int row = 0; row < d.size(); row++)
        for (int col = 0; col < d[0].size(); col++) {
            if (d[row][col] == '@' || d[row][col] == '+') {
                if (pos_player == make_pair(-1, -1))
                    pos_player = { row, col };
                else
                    is_invalid = true;
            }
            if (d[row][col] == '.' || d[row][col] == '*' || d[row][col] == '+')
                pos_storages.push_back({ row, col });
            if (d[row][col] == '$' || d[row][col] == '*')
                pos_boxes.push_back({ row, col });
        }
    if (pos_player == make_pair(-1, -1))
        is_invalid = true;
}

bool solver::is_movable(int const row, int const col, pair<int, int> const& dir) {
    int new_row = row + dir.first,
        new_col = col + dir.second;
    if (new_row < 0 || new_row >= this->stage_data.size() ||
        new_col < 0 || new_col >= this->stage_data[0].size())
        return false;

    if (this->stage_data[new_row][new_col] == ' ' || this->stage_data[new_row][new_col] == '.')
        return true;
    if (this->stage_data[row][col] != '$' && this->stage_data[row][col] != '*') {
        if (this->stage_data[new_row][new_col] == '$' && this->is_movable(new_row, new_col, dir))
            return true;
        if (this->stage_data[new_row][new_col] == '*' && this->is_movable(new_row, new_col, dir))
            return true;
    }
    return false;
}

int solver::move(pair<int, int> const& dir) {
    int move_cost = 0;
    int row = this->pos_player.first,
        col = this->pos_player.second;
    if (!this->is_movable(row, col, dir))
        return move_cost;
    int new_row = row + dir.first,
        new_col = col + dir.second;
    bool flag_pushed = false;
    move_cost = 3;
    if (this->stage_data[new_row][new_col] == ' ')
        this->stage_data[new_row][new_col] = '@';
    else if (this->stage_data[new_row][new_col] == '.')
        this->stage_data[new_row][new_col] = '+';
    else if (this->is_movable(new_row, new_col, dir)) {
        flag_pushed = true;
        if (this->stage_data[new_row][new_col] == '$')
            this->stage_data[new_row][new_col] = '@';
        else if (this->stage_data[new_row][new_col] == '*')
            this->stage_data[new_row][new_col] = '+';
    }

    if (flag_pushed) {
        if (this->stage_data[new_row + dir.first][new_col + dir.second] == ' ') {
            this->stage_data[new_row + dir.first][new_col + dir.second] = '$';
            move_cost = 2;
        }
        else {
            this->stage_data[new_row + dir.first][new_col + dir.second] = '*';
            move_cost = 0;
        }
        size_t box_i = find(this->pos_boxes.begin(), this->pos_boxes.end(), make_pair(new_row, new_col)) -
            this->pos_boxes.begin();
        this->pos_boxes[box_i] = { new_row + dir.first , new_col + dir.second };
    }

    if (this->stage_data[row][col] == '@')
        this->stage_data[row][col] = ' ';
    else
        this->stage_data[row][col] = '.';
    this->pos_player = { new_row, new_col };
    return move_cost;
}

bool solver::is_solved() const {
    for (auto& pos_box : pos_boxes)
        if (this->stage_data[pos_box.first][pos_box.second] != '*')
            return false;
    return true;
}

bool solver::is_stuck(int const row, int const col) const {
    return false;
}

bool solver::is_deadlock() const {
    int num_boxes = 0;
    for (int row = 0; row < this->stage_data.size(); row++)
        for (int col = 0; col < this->stage_data[0].size(); col++) {
            if (this->stage_data[row][col] == '*') {
                num_boxes++;
                continue;
            }
            if (this->stage_data[row][col] != '$')
                continue;
            num_boxes++;
            bool unmovable = false;
            for (int i = 0; i < 5; i++) {
                int new_row = row + dirs[i % 4].first,
                    new_col = col + dirs[i % 4].second;
                if (new_row < 0 || new_row >= this->stage_data.size() || new_col < 0 || new_col >= this->stage_data[0].size() ||
                    (this->stage_data[new_row][new_col] == '#')) {
                    if (unmovable) {
                        num_boxes--;
                        break;
                    }
                    else
                        unmovable = true;
                }
                else
                    unmovable = false;
            }
        }
    if (num_boxes < this->pos_storages.size())
        return true;
    else
        return false;
}

string solver::get_string() const {
    string ret;
    for (string const& s : this->stage_data)
        ret += s;
    return ret;
}

string solver::solve_bfs() const {
    if (this->is_invalid)
        return "INVALID MAP DATA";

    struct state {
        solver t;
        string log;

        state(solver const& t, string log)
            : t(t), log(log) {}
    };

    solver t = *this;
    queue<state> q;
    unordered_set<string> visited;

    state next_state(t, "");
    q.push(next_state);
    visited.insert(next_state.t.get_string());

    while (!q.empty()) {
        state cur_state = q.front();
        q.pop();
        if (cur_state.t.is_solved())
            return cur_state.log;
        if (cur_state.t.is_deadlock())
            continue;

        for (auto d : dirs) {
            if (cur_state.t.is_movable(cur_state.t.pos_player.first, cur_state.t.pos_player.second, d)) {
                state next_state(cur_state.t, cur_state.log);
                next_state.t.move(d);
                next_state.log += dir_to_char(d);
                string table_string = next_state.t.get_string();
                if (visited.find(table_string) == visited.end()) {
                    q.push(next_state);
                    visited.insert(table_string);
                }
            }
        }
    }

    return "NO SOLUTION";

}

int solver::manhattan_sum() const {
    int boxes_cost = this->pos_boxes.size() * this->stage_data.size() * this->stage_data[0].size();
    int player_cost = 0, min_player = INT_MAX;
    for (pair<int, int> const& box : this->pos_boxes) {
        int min_box = INT_MAX;
        for (pair<int, int> const& storage : this->pos_storages)
            min_box = min(min_box, abs(box.first - storage.first) + abs(box.second - storage.second));
        boxes_cost += min_box;

        min_player = min(min_player, abs(box.first - this->pos_player.first) + abs(box.second - this->pos_player.second));
    }
    player_cost += min_player;
    return boxes_cost + player_cost;
}

string solver::solve_astar() const {
    if (this->is_invalid)
        return "INVALID MAP DATA";

    struct state {
        solver t;
        string log;
        int g, h;

        state(solver const& t, string const& log, int g, int h)
            : t(t), log(log), g(g), h(h) {}

        bool operator<(state const& s) const {
            return this->g + this->h > s.g + s.h;
        }
    };

    solver t = *this;
    priority_queue<state> q;
    unordered_set<string> visited;

    state next_state(t, "", 0, 0);
    q.push(next_state);
    visited.insert(next_state.t.get_string());

    while (!q.empty()) {
        state cur_state = q.top();
        q.pop();
        if (cur_state.t.is_solved())
            return cur_state.log;
        if (cur_state.t.is_deadlock())
            continue;

        for (auto d : dirs) {
            if (cur_state.t.is_movable(cur_state.t.pos_player.first, cur_state.t.pos_player.second, d)) {
                state next_state(cur_state.t, cur_state.log, cur_state.g, 0);
                next_state.g += next_state.t.move(d);
                next_state.h = next_state.t.manhattan_sum();
                next_state.log += dir_to_char(d);
                string table_string = next_state.t.get_string();
                if (visited.find(table_string) == visited.end()) {
                    q.push(next_state);
                    visited.insert(table_string);
                }
            }
        }
    }

    return "NO SOLUTION";

}
