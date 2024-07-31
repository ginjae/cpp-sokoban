#include "block.h"
#include "table.h"
#include <iostream>
#include <Windows.h>


using namespace std;


void gotorc(int r, int c);


void player::move(table& tbl, pair<int, int> const& dir, bool update_flag) {
    if (this->is_movable(tbl, dir)) {
        int new_row = this->row + dir.first,
            new_col = this->col + dir.second;
        block* dest = tbl.get_block(new_row, new_col);
        if (dest != nullptr && dest->is_movable(tbl, dir))
            dest->move(tbl, dir, update_flag);

        tbl.set_block(new_row, new_col, this);
        if (update_flag) {
            gotorc(new_row, new_col);
            if (tbl.has_storage_at(new_row, new_col))
                cout << PLAYER_S;
            else
                cout << PLAYER;
        }

        tbl.set_block(this->row, this->col, nullptr);
        if (update_flag) {
            gotorc(this->row, this->col);
            if (tbl.has_storage_at(this->row, this->col))
                cout << STORAGE;
            else
                cout << FREE;
        }
        this->row = new_row;
        this->col = new_col;
    }
}

bool player::is_movable(table& tbl, pair<int, int> const& dir) const {
    int new_row = this->row + dir.first,
        new_col = this->col + dir.second;
    if (new_row < 0 || new_row >= tbl.get_height() || new_col < 0 || new_col >= tbl.get_width())
        return false;
    block* dest = tbl.get_block(new_row, new_col);
    if (dest == nullptr)
        return true;
    if (dest->is_movable(tbl, dir))
        return true;
    return false;
}

void box::move(table& tbl, pair<int, int> const& dir, bool update_flag) {
    if (this->is_movable(tbl, dir)) {
        int new_row = this->row + dir.first,
            new_col = this->col + dir.second;
        tbl.set_block(new_row, new_col, this);
        if (update_flag) {
            gotorc(new_row, new_col);
            if (tbl.has_storage_at(new_row, new_col))
                cout << BOX_S;
            else
                cout << BOX;
        }

        tbl.set_block(this->row, this->col, nullptr);
        if (update_flag) {
            gotorc(this->row, this->col);
            if (tbl.has_storage_at(this->row, this->col))
                cout << STORAGE;
            else
                cout << FREE;
        }
        this->row = new_row;
        this->col = new_col;

        if (update_flag) {
            gotorc(tbl.get_height(), 0);
            if (tbl.is_solved())
                cout << "CLEAR";
            else if (tbl.is_stuck())
                cout << "STUCK";
            else
                cout << "     ";
        }
    }
}

bool box::is_movable(table& tbl, pair<int, int> const& dir) const {
    int new_row = this->row + dir.first,
        new_col = this->col + dir.second;
    if (new_row < 0 || new_row >= tbl.get_height() || new_col < 0 || new_col >= tbl.get_width())
        return false;
    block* dest = tbl.get_block(new_row, new_col);
    if (dest == nullptr)
        return true;
    return false;
}

