#include <fstream>
#include <string>
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <filesystem>
#include "block.h"
#include "table.h"
#include "solver.h"


#define MAGIC_KEY   224
#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77

using namespace std;


pair<int, int> dirs[4] = {
    {-1, 0},   // UP
    {0, 1},    // RIGHT
    {1, 0},    // DOWN
    {0, -1}    // LEFT
};

constexpr int UP = 0;
constexpr int RIGHT = 1;
constexpr int DOWN = 2;
constexpr int LEFT = 3;

char dir_to_char(pair<int, int> const& dir) {
    if (dir == dirs[UP])
        return 'U';
    if (dir == dirs[RIGHT])
        return 'R';
    if (dir == dirs[DOWN])
        return 'D';
    if (dir == dirs[LEFT])
        return 'L';
    return '?';
}


void gotorc(int r, int c) {
    COORD cur;
    cur.X = c;
    cur.Y = r;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cur);
}


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
    "| [s] - solve the stage                           |",
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
            int height = 0, width = 0;
            vector<string> stage_data;
            string line;
            while (readFile.peek() != EOF) {
                getline(readFile, line);
                height++;
                width = max(width, line.size());
                stage_data.push_back(line);
            }
            for (string& line : stage_data) {
                if (line.size() > width)
                    line = line.substr(0, width);
                while (line.size() < width)
                    line += ' ';
            }
            stages_data.push_back(stage_data);
            stages_height.push_back(height);
            stages_width.push_back(width);
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
            if (t->is_invalid) {
                int discard = _getch();
                continue;
            }
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

            for (int i = 0; i <= MAX_MAP_HEIGHT + 3; i++) {
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

            for (int i = 0; i <= MAX_MAP_HEIGHT + 3; i++) {
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

            for (int i = 0; i <= MAX_MAP_HEIGHT + 3; i++) {
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
        else if (input == 115) {    // s
            delete t;
            t = new table(stages_height[current_stage], stages_width[current_stage], stages_data[current_stage]);
            for (int i = 0; i <= MAX_MAP_HEIGHT + 3; i++) {
                gotorc(i, 0);
                cout << string(MAX_MAP_WIDTH, ' ');
            }
            gotorc(0, 0);
            t->draw_table();
            if (t->is_invalid)
                continue;
            gotorc(t->get_height(), 0);
            cout << "SEARCHING...  ";
            solver s(stages_data[current_stage]);
            clock_t solver_start = clock();
            string solution = s.solve_bfs();
            clock_t solver_end = clock();
            gotorc(t->get_height(), 0);
            cout << "SOLUTION FOUND";
            gotorc(t->get_height() + 1, 0);
            cout << (double)(solver_end - solver_start) / CLOCKS_PER_SEC << "s";
            if (solution == "NO SOLUTION") {
                gotorc(t->get_height(), 0);
                cout << "NO SOLUTION   ";
                continue;
            }
            else {
                gotorc(t->get_height() + 2, 0);
                cout << solution.length() << " moves";
            }
            for (char c : solution) {
                if (c == 'U')
                    t->get_player()->move(*t, dirs[UP]);
                else if (c == 'D')
                    t->get_player()->move(*t, dirs[DOWN]);
                else if (c == 'L')
                    t->get_player()->move(*t, dirs[LEFT]);
                else
                    t->get_player()->move(*t, dirs[RIGHT]);
                Sleep(200);
            }
        }
//        else
//            cout << input << endl;
    }
    delete t;

	return 0;
}