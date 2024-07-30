#include "table.h"
#include <iostream>
#include <Windows.h>
#include <queue>
#include <unordered_set>

using namespace std;

void gotorc(int r, int c);
extern pair<int, int> dirs[4];

table::table(int const h, int const w, vector<string> const& data)
    : height(h), width(w), ptr_player(nullptr) {
    if (h < 0 || h > MAX_MAP_HEIGHT || w < 0 || w > MAX_MAP_WIDTH)
        this->is_invalid = true;
    for (int row = 0; row < h; row++) {
        vector<block*> v;
        for (int col = 0; col < w; col++) {
            switch (data[row][col]) {
            case PLAYER:
                if (this->ptr_player != nullptr) {
                    this->is_invalid = true;
                    v.push_back(nullptr);
                }
                else {
                    this->ptr_player = new player(row, col);
                    this->is_invalid = false;
                    v.push_back(this->ptr_player);
                }
                break;
            case WALL:
                v.push_back(new wall(row, col));
                break;
            case BOX:
                v.push_back(new box(row, col));
                break;
            case STORAGE:
                this->storages.push_back(make_pair(row, col));
                v.push_back(nullptr);
                break;
            case PLAYER_S:
                if (this->ptr_player != nullptr) {
                    this->is_invalid = true;
                    v.push_back(nullptr);
                }
                else {
                    this->ptr_player = new player(row, col);
                    this->is_invalid = false;
                    v.push_back(this->ptr_player);
                }
                this->storages.push_back(make_pair(row, col));
                break;
            case BOX_S:
                this->storages.push_back(make_pair(row, col));
                v.push_back(new box(row, col));
                break;
            case FREE:
                v.push_back(nullptr);
                break;
            default:
                v.push_back(nullptr);
            }
        }
        this->map.push_back(v);
    }
}

table::~table() {
    for (int row = 0; row < this->height; row++)
        for (int col = 0; col < this->width; col++)
            if (this->map[row][col] != nullptr)
                delete this->map[row][col];
}

table::table(table const& other)
    : height(other.height), width(other.width), ptr_player(nullptr),
        storages(other.storages), is_invalid(other.is_invalid) {
    for (int row = 0; row < other.height; row++) {
        vector<block*> v;
        for (int col = 0; col < other.width; col++) {
            if (other.map[row][col] != nullptr) {
                block* b = other.map[row][col]->copy();
                v.push_back(b);
                if (b->get_char() == PLAYER)
                    this->ptr_player = (player*)b;
            }
            else
                v.push_back(nullptr);
        }
        this->map.push_back(v);
    }
}

table& table::operator=(table const& other) {
    for (int row = 0; row < this->height; row++)
        for (int col = 0; col < this->width; col++)
            if (this->map[row][col] != nullptr)
                delete this->map[row][col];

    this->height = other.height;
    this->width = other.width;
    this->ptr_player = nullptr;
    this->storages = other.storages;
    this->is_invalid = other.is_invalid;

    this->map.clear();

    for (int row = 0; row < other.height; row++) {
        vector<block*> v;
        for (int col = 0; col < other.width; col++) {
            if (other.map[row][col] != nullptr) {
                block* b = other.map[row][col]->copy();
                v.push_back(b);
                if (b->get_char() == PLAYER)
                    this->ptr_player = (player*)b;
            }
            else
                v.push_back(nullptr);
        }
        this->map.push_back(v);
    }

    return *this;
}

void table::draw_table() const {
    if (this->is_invalid || this->ptr_player == nullptr) {
        cout << "INVALID MAP DATA" << '\n';
        return;
    }
    for (int row = 0; row < this->height; row++) {
        gotorc(row, 0);
        for (int col = 0; col < this->width; col++) {
            if (this->map[row][col] == nullptr)
                cout << FREE;
            else
                cout << this->map[row][col]->get_char();
        }
    }

    for (pair<int, int> const& s : this->storages) {
        gotorc(s.first, s.second);
        if (this->map[s.first][s.second] == nullptr)
            cout << STORAGE;
        else if (this->map[s.first][s.second]->get_char() == PLAYER)
            cout << PLAYER_S;
        else if (this->map[s.first][s.second]->get_char() == BOX)
            cout << BOX_S;
        else if (this->map[s.first][s.second]->get_char() == WALL)
            continue;
    }
    gotorc(this->height, 0);
    if (this->is_solved())
        cout << "CLEAR";
}

bool table::has_storage_at(int const r, int const c) const {
    if (find(this->storages.begin(), this->storages.end(), make_pair(r, c)) == this->storages.end())
        return false;
    return true;
}

bool table::is_solved() const {
    for (pair<int, int> const& storage : this->storages) {
        if (this->map[storage.first][storage.second] != nullptr &&
                this->map[storage.first][storage.second]->get_char() == BOX)
                continue;
        else
            return false;
    }
    return true;
}

bool table::is_stuck() const {
    size_t num_boxes = 0;
    for (int row = 0; row < this->height; row++)
        for (int col = 0; col < this->width; col++) {
            if (this->map[row][col] == nullptr || this->map[row][col]->get_char() != BOX)
                continue;
            num_boxes++;
            if (this->has_storage_at(row, col))
                continue;
            bool unmovable = false;
            for (int i = 0; i < 5; i++) {
                int new_row = row + dirs[i % 4].first,
                    new_col = col + dirs[i % 4].second;
                if (new_row < 0 || new_row >= this->height || new_col < 0 || new_col >= this->width ||
                        (this->map[new_row][new_col] != nullptr && this->map[new_row][new_col]->get_char() == WALL)) {
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
    if (num_boxes < this->storages.size())
        return true;
    else
        return false;
}

//string table::solve_bfs() const {
//    if (this->is_invalid || this->ptr_player == nullptr)
//        return "INVALID MAP DATA";
//
//    struct state {
//        table t;
//        pair<int, int> dir;
//        vector<pair<int, int>> prev_pos;
//        vector<bool> prev_result;
//    };
//
//    table t = *this;
//    queue<state*> q;
//    for (auto d : dirs)
//        if (t.ptr_player->is_movable(t, d)) {
//            vector<pair<int, int>> pos;
//            pos.push_back(make_pair(t.ptr_player->get_row(), t.ptr_player->get_col()));
//            state* next_state = new state{ t, d, pos, {} };
//            next_state.prev_result.push_back(next_state.t.ptr_player->move(next_state.t, d, false));
//            q.push(next_state);
//        }
//
//    while (!q.empty()) {
//        state* cur_state = q.front();
//        if (cur_state.t.is_solved()) {
//            string result;
//            for (size_t i = 1; i <= cur_state.prev_pos.size(); i++) {
//                int vertical, horizontal;
//                if (i == cur_state.prev_pos.size()) {
//                    vertical = cur_state.t.get_player()->get_row() - cur_state.prev_pos[i - 1].first;
//                    horizontal = cur_state.t.get_player()->get_col() - cur_state.prev_pos[i - 2].second;
//                }
//                else {
//                    vertical = cur_state.prev_pos[i].first - cur_state.prev_pos[i - 1].first;
//                    horizontal = cur_state.prev_pos[i].second - cur_state.prev_pos[i - 1].second;
//                }
//                if (vertical == 0) {
//                    if (horizontal == 1)
//                        result += 'R';
//                    else
//                        result += 'L';
//                }
//                else if (vertical == -1)
//                    result += 'U';
//                else
//                    result += 'D';
//            }
//            while (!q.empty()) {
//                delete q.front();
//                q.pop();
//            }
//            return result;
//        }
//        if (cur_state.t.is_stuck()) {
//            delete q.front();
//            q.pop();
//            continue;
//        }
//        size_t idx = find(cur_state.prev_pos.begin(), cur_state.prev_pos.end(),
//            make_pair(cur_state.t.ptr_player->get_row(), cur_state.t.ptr_player->get_col())) - cur_state.prev_pos.begin();
//        if (idx < cur_state.prev_pos.size()) {
//            size_t i;
//            for (i = idx; i < cur_state.prev_result.size(); i++)
//                if (cur_state.prev_result[i])
//                    break;
//            if (i == cur_state.prev_result.size()) {
//                delete q.front();
//                q.pop();
//                continue;
//            }
//        }
//        for (auto d : dirs)
//            if (cur_state.t.ptr_player->is_movable(cur_state.t, d)) {
//                state* next_state = new state{ cur_state.t, d, cur_state.prev_pos, cur_state.prev_result };
//                next_state.prev_result.push_back(next_state.t.ptr_player->move(next_state.t, d, false));
//                next_state.prev_pos.push_back(make_pair(cur_state.t.ptr_player->get_row(), cur_state.t.ptr_player->get_col()));
//                q.push(next_state);
//            }
//        //cout << cur_state.t.get_player()->get_row() << ", " << cur_state.t.get_player()->get_col() << endl;
//        delete q.front();
//        q.pop();
//    }
//
//    return "NO SOLUTION";
//}
//

string table::get_string() const {
    string s;
    for (int row = 0; row < this->height; row++)
        for (int col = 0; col < this->width; col++) {
            if (this->map[row][col] == nullptr)
                s += FREE;
            else
                s += this->map[row][col]->get_char();
        }
    return s;
}

//string table::solve_bfs() const {
//    if (this->is_invalid || this->ptr_player == nullptr)
//        return "INVALID MAP DATA";
//
//    struct state {
//        table t;
//        pair<int, int> dir;
//        vector<pair<int, int>> prev_pos;
//        vector<bool> prev_result;
//    };
//
//    table t = *this;
//    queue<state> q;
//    unordered_set<string> visited;
//
//    for (auto d : dirs) {
//        if (t.ptr_player->is_movable(t, d)) {
//            vector<pair<int, int>> pos;
//            pos.push_back(make_pair(t.ptr_player->get_row(), t.ptr_player->get_col()));
//            state next_state = { t, d, pos, {} };
//            next_state.prev_result.push_back(next_state.t.ptr_player->move(next_state.t, d, false));
//            q.push(next_state);
//            visited.insert(next_state.t.get_string());
//        }
//    }
//
//    while (!q.empty()) {
//        state& cur_state = q.front();
//        if (cur_state.t.is_solved()) {
//            string result;
//            for (size_t i = 1; i <= cur_state.prev_pos.size(); i++) {
//                int vertical, horizontal;
//                if (i == cur_state.prev_pos.size()) {
//                    vertical = cur_state.t.get_player()->get_row() - cur_state.prev_pos[i - 1].first;
//                    horizontal = cur_state.t.get_player()->get_col() - cur_state.prev_pos[i - 2].second;
//                }
//                else {
//                    vertical = cur_state.prev_pos[i].first - cur_state.prev_pos[i - 1].first;
//                    horizontal = cur_state.prev_pos[i].second - cur_state.prev_pos[i - 1].second;
//                }
//                if (vertical == 0) {
//                    if (horizontal == 1)
//                        result += 'R';
//                    else
//                        result += 'L';
//                }
//                else if (vertical == -1)
//                    result += 'U';
//                else
//                    result += 'D';
//            }
//            return result;
//        }
//        if (cur_state.t.is_stuck()) {
//            q.pop();
//            continue;
//        }
//
//        for (auto d : dirs) {
//            if (cur_state.t.ptr_player->is_movable(cur_state.t, d)) {
//                state next_state = { cur_state.t, d, cur_state.prev_pos, cur_state.prev_result };
//                next_state.prev_result.push_back(next_state.t.ptr_player->move(next_state.t, d, false));
//                next_state.prev_pos.push_back(make_pair(cur_state.t.ptr_player->get_row(), cur_state.t.ptr_player->get_col()));
//                string table_string = next_state.t.get_string();
//                if (visited.find(table_string) == visited.end()) {
//                    q.push(next_state);
//                    visited.insert(table_string);
//                }
//            }
//        }
//
//        q.pop();
//    }
//
//    return "NO SOLUTION";
//}
//

string table::solve_bfs() const {
    if (this->is_invalid || this->ptr_player == nullptr)
        return "INVALID MAP DATA";

    struct state {
        table t;
        pair<int, int> dir;
        vector<pair<int, int>> prev_pos;
        vector<bool> prev_result;
    };

    table t = *this;
    queue<state*> q;
    unordered_set<string> visited;

    for (auto d : dirs) {
        if (t.ptr_player->is_movable(t, d)) {
            vector<pair<int, int>> pos;
            pos.push_back(make_pair(t.ptr_player->get_row(), t.ptr_player->get_col()));
            state* next_state = new state{ t, d, pos, {} };
            next_state->prev_result.push_back(next_state->t.ptr_player->move(next_state->t, d, false));
            q.push(next_state);
            visited.insert(next_state->t.get_string());
        }
    }

    while (!q.empty()) {
        state* cur_state = q.front();
        if (cur_state->t.is_solved()) {
            string result;
            for (size_t i = 1; i <= cur_state->prev_pos.size(); i++) {
                int vertical, horizontal;
                if (i == cur_state->prev_pos.size()) {
                    vertical = cur_state->t.get_player()->get_row() - cur_state->prev_pos[i - 1].first;
                    horizontal = cur_state->t.get_player()->get_col() - cur_state->prev_pos[i - 2].second;
                }
                else {
                    vertical = cur_state->prev_pos[i].first - cur_state->prev_pos[i - 1].first;
                    horizontal = cur_state->prev_pos[i].second - cur_state->prev_pos[i - 1].second;
                }
                if (vertical == 0) {
                    if (horizontal == 1)
                        result += 'R';
                    else
                        result += 'L';
                }
                else if (vertical == -1)
                    result += 'U';
                else
                    result += 'D';
            }
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
            if (cur_state->t.ptr_player->is_movable(cur_state->t, d)) {
                state* next_state = new state{ cur_state->t, d, cur_state->prev_pos, cur_state->prev_result };
                next_state->prev_result.push_back(next_state->t.ptr_player->move(next_state->t, d, false));
                next_state->prev_pos.push_back(make_pair(cur_state->t.ptr_player->get_row(), cur_state->t.ptr_player->get_col()));
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
