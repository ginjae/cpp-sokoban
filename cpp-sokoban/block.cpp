#include "block.h"
#include "table.h"
#include <iostream>
#include <Windows.h>

using namespace std;

void gotorc(int r, int c) {
    COORD cur;
    cur.X = c;
    cur.Y = r;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cur);
}

void player::move(table& tbl, pair<int, int> const& dir) {
    if (tbl.invalid_flag)
        return;
    int new_row = this->row + dir.first,
        new_col = this->col + dir.second;
    if (new_row < 0 || new_row >= tbl.get_height() ||
            new_col < 0 || new_col >= tbl.get_width())
        return;

    
//    CONSOLE_SCREEN_BUFFER_INFO previous_cur;
//    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &previous_cur);

    block* target = tbl.map[new_row][new_col];

    if (target == nullptr || target->is_pushable(tbl, *tbl.get_player())) {

        if (target != nullptr && target->get_char() == BOX) {

            target->set_row(new_row + dir.first);
            target->set_col(new_col + dir.second);
            tbl.map[new_row + dir.first][new_col + dir.second] = tbl.map[new_row][new_col];
            tbl.map[new_row][new_col] = nullptr;

            gotorc(new_row, new_col);
            if (tbl.has_storage_at(new_row, new_col))
                cout << STORAGE;
            else
                cout << FREE;

            gotorc(new_row + dir.first, new_col + dir.second);
            if (tbl.has_storage_at(new_row + dir.first, new_col + dir.second))
                cout << BOX_S;
            else
                cout << BOX;
        }

        tbl.map[new_row][new_col] = tbl.map[this->row][this->col];
        tbl.map[this->row][this->col] = nullptr;

        // 1. remove character on console
        gotorc(this->row, this->col);
        if (tbl.has_storage_at(this->row, this->col))
            cout << STORAGE;
        else
            cout << FREE;
        // 2. write character on console
        gotorc(new_row, new_col);
        if (tbl.has_storage_at(new_row, new_col))
            cout << PLAYER_S;
        else
            cout << PLAYER;

//        gotorc(previous_cur.dwCursorPosition.Y, previous_cur.dwCursorPosition.X);
        gotorc(tbl.get_height(), 0);
        if (tbl.solved_flag == false && tbl.is_solved()) {
            tbl.solved_flag = true;
            cout << "CLEAR!";
        }
        this->row = new_row;
        this->col = new_col;
    }
}

bool box::is_pushable(table const& tbl, block const& other) {
    pair<int, int> dir = make_pair(
        this->row - other.get_row(),
        this->col - other.get_col()
    );
//    cout << dir.first << ", " << dir.second << endl;
    if (this->row + dir.first < 0 || this->row + dir.first >= tbl.get_height() ||
            this->col + dir.second < 0 || this->col + dir.second >= tbl.get_width())
        return false;
    if (tbl.map[this->row + dir.first][this->col + dir.second] == nullptr)
        return true;
    return false;
//    char item_behind = tbl.map[this->row + dir.first][this->col + dir.second]->get_char();
//    if (item_behind == WALL || item_behind == BOX)
//        return false;
//  return true;
}

