#include <fstream>
#include <string>
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <filesystem>
#include "block.h"
#include "table.h"


#define MAGIC_KEY   224
#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77


#define UP      0
#define DOWN    1
#define LEFT    2
#define RIGHT   3

using namespace std;

pair<int, int> dirs[4] = {
    make_pair(-1, 0),   // UP
    make_pair(1, 0),    // DOWN
    make_pair(0, -1),   // LEFT
    make_pair(0, 1)     // RIGHT
};


void gotorc(int r, int c);


const string MANUAL[] = {
    "+-------------------------------------------------+",
    "| '#' - a wall                                    |",
    "| ' ' - free square                               |",
    "| '.' - free storage location                     |",
    "| '@' - the player                                |",
    "| '+' - the player standing on a storage location |",
    "| '$' - a box                                     |",
    "| '*' - a box placed on a storage location        |",
    "+-------------------------------------------------+",
    "|                    CONTROLS                     |",
    "| arrows - move                                   |",
    "| [r] - restart the stage                         |",
    "| [n] - go to next stage                          |",
    "| [b] - go to previous stage                      |",
    "| [e] - exit                                      |",
    "+-------------------------------------------------+"
};


void init(vector<string> const& path_stages) {
//    SetConsoleTitle(TEXT("SOKOBAN"));

    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO console_cursor;
    console_cursor.bVisible = false;
    console_cursor.dwSize = 1;
    SetConsoleCursorInfo(console_handle, &console_cursor);
    for (size_t i = 0; i <= path_stages.size() + 1; i++) {
        gotorc((int)i, MAX_MAP_WIDTH + 2);
        if (i == 0)
            cout << "+" << string(5, '-') << "STAGES" << string(5, '-') << "+";
        else if (i == path_stages.size() + 1)
            cout << "+" << string(16, '-') << "+";
        else {
            string s = path_stages[i - 1].substr(path_stages[i - 1].find("stages") + 7, 15);
            if (s.size() > 14)
                s = s.substr(0, 10) + "... ";
            else
                while (s.size() < 14)
                    s += " ";
            cout << "|  " << s << "|";
        }
    }

    for (size_t i = 0; i < sizeof MANUAL / sizeof MANUAL[0]; i++) {
        gotorc((int)i, MAX_MAP_WIDTH + 21);
        cout << MANUAL[i];
    }
    gotorc(0, 0);
}

int main() {
    vector<string> path_stages;
    if (!filesystem::is_directory(filesystem::current_path() / "stages")) {
        cerr << "ERROR: no 'stages' directory" << endl;
        return 1;
    }
    filesystem::directory_iterator itr("./stages");
    while (itr != filesystem::end(itr)) {
        const filesystem::directory_entry& entry = *itr;
        path_stages.push_back(entry.path().string());
        itr++;
    }
    if (path_stages.size() == 0) {
        cerr << "ERROR: no stage data" << endl;
        return 1;
    }

    int current_stage = 0;
    vector<vector<string>> stages_data;
    vector<int> stages_height, stages_width;
    for (int stage_i = 0; stage_i < path_stages.size(); stage_i++) {
        ifstream readFile(path_stages[stage_i]);
        if (readFile.is_open()) {
            int height, width;
            readFile >> height >> width;
            stages_height.push_back(height);
            stages_width.push_back(width);
            readFile.get(); // get newline char
            vector<string> stage_data;
            for (int i = 0; i < height; i++) {
                string str;
                getline(readFile, str);
                if (str.size() > height)
                    str.substr(0, height);
                while (str.size() < width) {
                    str += ' ';
                }
                stage_data.push_back(str);
            }
            stages_data.push_back(stage_data);
            readFile.close();
        }
    }
    init(path_stages);
    table* t = new table(stages_height[current_stage], stages_width[current_stage], stages_data[current_stage]);
    t->draw_table();
    gotorc(current_stage + 1, MAX_MAP_WIDTH + 3);
    cout << "->";

    int input;
    while (1) {
        input = _getch();
        if (input == MAGIC_KEY) {
            switch (_getch()) {
            case KEY_UP:
                t->get_player()->move(*t, dirs[UP]);
                break;
            case KEY_DOWN:
                t->get_player()->move(*t, dirs[DOWN]);
                break;
            case KEY_LEFT:
                t->get_player()->move(*t, dirs[LEFT]);
                break;
            case KEY_RIGHT:
                t->get_player()->move(*t, dirs[RIGHT]);
                break;
            }
        }
        else if (input == 114) {    // r
            delete t;

            for (int i = 0; i <= MAX_MAP_HEIGHT; i++) {
                gotorc(i, 0);
                cout << string(MAX_MAP_WIDTH, ' ');
            }

            gotorc(0, 0);
            t = new table(stages_height[current_stage], stages_width[current_stage], stages_data[current_stage]);
            t->draw_table();
        }
        else if (input == 101)      // e
            break;
        else if (input == 110) {    // n
            if (current_stage == path_stages.size() - 1)
                continue;
            delete t;

            for (int i = 0; i <= MAX_MAP_HEIGHT; i++) {
                gotorc(i, 0);
                cout << string(MAX_MAP_WIDTH, ' ');
            }

            gotorc(current_stage + 1, MAX_MAP_WIDTH + 3);
            cout << "  ";
            
            current_stage++;

            gotorc(current_stage + 1, MAX_MAP_WIDTH + 3);
            cout << "->";

            gotorc(0, 0);
            t = new table(stages_height[current_stage], stages_width[current_stage], stages_data[current_stage]);
            t->draw_table();
        }
        else if (input == 98) {    // b
            if (current_stage == 0)
                continue;
            delete t;

            for (int i = 0; i <= MAX_MAP_HEIGHT; i++) {
                gotorc(i, 0);
                cout << string(MAX_MAP_WIDTH, ' ');
            }

            gotorc(current_stage + 1, MAX_MAP_WIDTH + 3);
            cout << "  ";

            current_stage--;


            gotorc(current_stage + 1, MAX_MAP_WIDTH + 3);
            cout << "->";

            gotorc(0, 0);
            t = new table(stages_height[current_stage], stages_width[current_stage], stages_data[current_stage]);
            t->draw_table();
        }
//        else
//            cout << input << endl;
    }
    delete t;

	return 0;
}