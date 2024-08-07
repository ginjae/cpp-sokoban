#include "solver.h"
#include <queue>
#include <unordered_set>

using namespace std;

extern pair<int, int> dirs[4];
char dir_to_char(pair<int, int> const& dir);

solver::solver(vector<string> const& d) : stage_data(d), pos_player({ -1, -1 }) {
    for (size_t row = 0; row < d.size(); row++)
        for (size_t col = 0; col < d[0].size(); col++) {
            if (d[row][col] == '@' || d[row][col] == '+') {
                if (pos_player == make_pair(-1, -1))
                    pos_player = { row, col };
                else
                    is_invalid = true;
            }
            if (d[row][col] == '.' || d[row][col] == '*' || d[row][col] == '+')
                pos_storages.push_back({ row, col });
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

void solver::move(pair<int, int> const& dir) {
    int row = this->pos_player.first,
        col = this->pos_player.second;
    if (!this->is_movable(row, col, dir))
        return;
    int new_row = row + dir.first,
        new_col = col + dir.second;
    bool flag_pushed = false;
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
    else
        return;

    if (flag_pushed) {
        if (this->stage_data[new_row + dir.first][new_col + dir.second] == ' ')
            this->stage_data[new_row + dir.first][new_col + dir.second] = '$';
        else
            this->stage_data[new_row + dir.first][new_col + dir.second] = '*';
    }

    if (this->stage_data[row][col] == '@')
        this->stage_data[row][col] = ' ';
    else
        this->stage_data[row][col] = '.';
    this->pos_player = { new_row, new_col };
}

bool solver::is_solved() const {
    for (auto& pos_storage : pos_storages)
        if (this->stage_data[pos_storage.first][pos_storage.second] != '*')
            return false;
    return true;
}

bool solver::is_stuck() const {
    size_t num_boxes = 0;
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
    };

    solver t = *this;
    queue<state*> q;
    unordered_set<string> visited;

    state* next_state = new state{ t, "" };
    q.push(next_state);
    visited.insert(next_state->t.get_string());

    while (!q.empty()) {
        state* cur_state = q.front();
        if (cur_state->t.is_solved()) {
            string result = cur_state->log;
            while (!q.empty()) {
                delete q.front();
                q.pop();
            }
            return result;
        }
        if (cur_state->t.is_stuck()) {
            delete q.front();
            q.pop();
            continue;
        }

        for (auto d : dirs) {
            if (cur_state->t.is_movable(cur_state->t.pos_player.first, cur_state->t.pos_player.second, d)) {
                state* next_state = new state{ cur_state->t, cur_state->log };
                next_state->t.move(d);
                next_state->log += dir_to_char(d);
                string table_string = next_state->t.get_string();
                if (visited.find(table_string) == visited.end()) {
                    q.push(next_state);
                    visited.insert(table_string);
                }
                else
                    delete next_state;
            }
        }
        delete q.front();
        q.pop();
    }

    return "NO SOLUTION";

}
