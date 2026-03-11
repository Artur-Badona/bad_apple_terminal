#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <curses.h>

using namespace std;

vector<vector<vector<int>>> get_frames(string file_name) {
    ifstream file(file_name);
    string line;

    vector<vector<int>> current_frame;
    vector<vector<vector<int>>> frames;

    while (getline(file, line)) {
        if (line.empty()) {
            if (!current_frame.empty()) {
                frames.push_back(std::move(current_frame));
                current_frame.clear();
            }
            continue;
        }

        vector<int> row;
        for (char c : line) {
            if (isdigit(c))
            row.push_back(c - '0');
        }

        if (!row.empty()) {
            current_frame.push_back(std::move(row));
        }
    }

    if (!current_frame.empty()) {
        frames.push_back(std::move(current_frame));
        current_frame.clear();
    }

    file.close();
    return frames;
}


#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <curses.h> // Ensure you link this with -lncurses

using namespace std;

// ... get_frames function remains the same ...

int main() {
    vector<vector<vector<int>>> frames = get_frames("frames_128.txt");
    if (frames.empty()) {
        cerr << "No frames loaded!" << endl;
        return 1;
    }

    // 1. Initialize ncurses
    initscr();            // Start curses mode
    noecho();             // Don't echo typed characters
    curs_set(0);          // Hide the blinking cursor
    timeout(0);           // Non-blocking input (optional)

    // Optional: Setup colors if your terminal supports it
    if (has_colors()) {
        start_color();
        // Pair 1: Red foreground, Black background (for 0)
        init_pair(1, COLOR_RED, COLOR_BLACK);
        // Pair 2: Red foreground, White background (for 1)
        init_pair(2, COLOR_RED, COLOR_WHITE);
    }

    for (const auto& frame : frames) {
        // 2. Clear the internal buffer (virtual screen)
        erase();

        int row_idx = 0;
        for (const auto& row : frame) {
            int col_idx = 0;
            for (int val : row) {
                // Pick color based on value
                attron(COLOR_PAIR(val == 0 ? 1 : 2));

                // Draw "pixels" (two spaces for a blockier look)
                mvaddstr(row_idx, col_idx * 2, "  ");

                attroff(COLOR_PAIR(val == 0 ? 1 : 2));
                col_idx++;
            }
            row_idx++;
        }

        // 3. Push the virtual screen to the physical terminal
        refresh();

        // 4. Frame rate control (microseconds)
        // 33000us is approx 30 FPS
        usleep(50000);
    }

    // 5. Clean up and exit
    printw("\nPress any key to exit...");
    refresh();
    getch();
    endwin();

    return 0;
}
