#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

const int ROWS = 45;
const int COLS = 35;

void modifyArray(char arr[ROWS][COLS], const std::vector<std::pair<int, int>>& indices) {
    for (const auto& index : indices) {
        int i = index.first;
        int j = index.second;
        std::stringstream ss;
        ss << "(" << std::setfill('0') << std::setw(2) << i << "," 
           << std::setfill('0') << std::setw(2) << j << ")";
        std::string replacement = ss.str();
        
        for (size_t k = 0; k < replacement.length(); ++k) {
            if (j + k < COLS) {
                arr[i][j + k] = replacement[k];
            }
        }
    }
}

void printArray(const char arr[ROWS][COLS]) {
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            std::cout << arr[i][j];
        }
        std::cout << '\n';
    }
}

int main() {
    char arr[ROWS][COLS] = {
        "          a     b     c           ",
        "         / \\   / \\   / \\          ",
        "                                  ",
        "        /   \\ /   \\ /   \\         ",
        "       1     2     3     4        ",
        "       |     |     |     |        ",
        "                                  ",
        "       |     |     |     |        ",
        "       d     e     f     g        ",
        "      / \\   / \\   / \\   / \\       ",
        "                                  ",
        "     /   \\ /   \\ /   \\ /   \\      ",
        "    5     6     7     8     9     ",
        "    |     |     |     |     |     ",
        "                                  ",
        "    |     |     |     |     |     ",
        "    h     i     j     k     l     ",
        "   / \\   / \\   / \\   / \\   / \\    ",
        "                                  ",
        "  /   \\ /   \\ /   \\ /   \\ /   \\   ",
        " 10    11    12    14    15    16 ",
        " |     |     |     |     |     |  ",
        "                                  ",
        " |     |     |     |     |     |  ",
        " m     n     o     p     q     r  ",
        "  \\   / \\   / \\   / \\   / \\   /   ",
        "                                  ",
        "   \\ /   \\ /   \\ /   \\ /   \\ /    ",
        "    1     2     3     4     5     ",
        "    |     |     |     |     |     ",
        "                                  ",
        "    |     |     |     |     |     ",
        "    s     t     u     v     w     ",
        "     \\   / \\   / \\   / \\   /      ",
        "                                  ",
        "      \\ /   \\ /   \\ /   \\ /       ",
        "       y     z     A     B        ",
        "       |     |     |     |        ",
        "                                  ",
        "       |     |     |     |        ",
        "       1     2     3     4        ",
        "        \\   / \\   / \\    /        ",
        "                                  ",
        "         \\ /   \\ /   \\ /          ",
        "          E     F     G           "
    };

    std::vector<std::pair<int, int>> indices = {
        {0, 10}, {0, 16}, {0, 22},
        {4, 7}, {4, 13}, {4, 19}, {4, 25},
        {8, 7}, {8, 13}, {8, 19}, {8, 25},
        {12, 4}, {12, 10}, {12, 16}, {12, 22}, {12, 28},
        {16, 4}, {16, 10}, {16, 16}, {16, 22}, {16, 28},
        {20, 1}, {20, 7}, {20, 13}, {20, 19}, {20, 25}, {20, 31},
        {24, 1}, {24, 7}, {24, 13}, {24, 19}, {24, 25}, {24, 31},
        {28, 4}, {28, 10}, {28, 16}, {28, 22}, {28, 28},
        {32, 4}, {32, 10}, {32, 16}, {32, 22}, {32, 28},
        {36, 7}, {36, 13}, {36, 19}, {36, 25},
        {40, 7}, {40, 13}, {40, 19}, {40, 25},
        {44, 10}, {44, 16}, {44, 22}
    };

    modifyArray(arr, indices);

    std::cout << "Modified array with index pairs replacing numbers and letters:\n";
    printArray(arr);

    return 0;
}