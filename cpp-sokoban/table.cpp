#include "table.h"
#include <iostream>
#include <Windows.h>

using namespace std;

void gotorc(int r, int c);

table::table(int const h, int const w, vector<string> const& data)
    : height(h), width(w), ptr_player(nullptr) {
    if (h < 0 || h > MAX_MAP_HEIGHT || w < 0 || w > MAX_MAP_WIDTH)
        this->invalid_flag = true;
    for (int row = 0; row < h; row++) {
        vector<block *> v;
        for (int col = 0; col < w; col++) {
            switch (data[row][col]) {
            case PLAYER:
                if (this->ptr_player != nullptr) {
                    this->invalid_flag = true;
                    v.push_back(nullptr);
                }
                else {
                    this->ptr_player = new player(row, col);
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
                    this->invalid_flag = true;
                    v.push_back(nullptr);
                }
                else {
                    this->ptr_player = new player(row, col);
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
                this->invalid_flag = false;   // error occur
            }
        }
        this->map.push_back(v);
    }
}

void table::draw_table() const {
    if (this->invalid_flag) {
        cout << "INVALID MAP DATA" << '\n';
        return;
    }
    for (int row = 0; row < this->height; row++) {
        for (int col = 0; col < this->width; col++) {
            if (this->map[row][col] == nullptr)
                cout << FREE;
            else
                cout << this->map[row][col]->get_char();
        }
        cout << '\n';
    }

    CONSOLE_SCREEN_BUFFER_INFO previous_cur;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &previous_cur);
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
    gotorc(previous_cur.dwCursorPosition.Y, previous_cur.dwCursorPosition.X);
    //cout << "num of rows: " << this->height << "\nnum of cols: " << this->width << endl;
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

table::~table() {
    for (int row = 0; row < this->height; row++)
        for (int col = 0; col < this->width; col++)
            if (this->map[row][col] != nullptr)
                delete this->map[row][col];
}
