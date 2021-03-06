#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <random>
#include <cstdlib>

using namespace std;

//                               Definision
// ***********************************************************************
#define BOARD_SIZE 10
#define MAX_TURN   100
#define COLOR      4

const char colorChar[] = { 'R', 'Y', 'G', 'B' };

enum PrintType {
    DEFAULT = 0,
    COLOR_NUMBER,
    COLOR_STRING,
};

enum Color {
    RED = 0,
    YELLOW,
    GREEN,
    BLUR
};

enum FontColorCode {
    FG_RED           = 31,
    FG_GREEN         = 32,
    FG_YELLOW        = 33,
    FG_BLUE          = 34,
    FG_MAGENTA       = 35,
    FG_CYAN          = 36,
    FG_LIGHT_GRAY    = 37,
    FG_DARK_GRAY     = 90,
    FG_LIGHT_RED     = 91,
    FG_LIGHT_GREEN   = 92,
    FG_LIGHT_YELLOW  = 93,
    FG_LIGHT_BLUE    = 94,
    FG_LIGHT_MAGENTA = 95,
    FG_LIGHT_CYAN    = 96,
    FG_WHITE         = 97,
    BG_RED           = 41,
    BG_GREEN         = 42,
    BG_BLUE          = 44,
    BG_DEFAULT       = 49
};

struct Point {
    int x;
    int y;
    Point(int _x, int _y) { x = _x; y = _y; }
};

class Board {
    private:
        int turn;
        int seed;
        int score;
        std::vector<std::vector<int>> board;
        bool replace(Point);
        void check();
    public:
        Board(int _seed = 0);
        Board(int, int, int, std::vector<std::vector<int>>);
        bool swap(Point, Point);
        int getCell(Point);
        int getTurn();
        int getScore();
        bool canSwap();
        void printBoard(int printType = 0);
        std::vector<std::vector<int>> getBoard();
        ~Board();
};

class XORShift {
    private:
        unsigned x = 123456789u, y = 362436069u, z = 521288629u, w;
        unsigned random();
    public:
        static unsigned min(){ return 0u; }
        static unsigned max(){ return UINT_MAX; }
        unsigned operator()(){ return random(); }
        XORShift() { std::random_device rd; x = 123456789u; y = 362436069u; z = 521288629u; w = w = rd(); }
        XORShift(unsigned seed) { x = 123456789u; y = 362436069u; z = 521288629u; w = seed; }
};

bool checkAlign(Point);
std::vector<std::vector<int>> makeBoard(int);
std::vector<int> getNextTile(unsigned int, unsigned int);
void printTile(std::vector<int> &, int printType = 0);
void printCell(int cell, int printType = 0);
// ***********************************************************************

//                               Implemention
// ***********************************************************************
Board::Board(int _seed) {
    turn  = 0;
    score = 0;
    seed  = _seed;
    board = makeBoard(_seed);
}

Board::Board(int _seed, int _turn, int _score, std::vector<std::vector<int>> _board) {
    turn  = _turn;
    score = _score;
    seed  = _seed;
    board = _board;
}

bool Board::swap(Point pos1, Point pos2) {
    if (!checkAlign(pos1) || !checkAlign(pos2)) return false;
    if ((abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y)) > 1) return false;
    if (turn++ >= MAX_TURN) return false;

    int tmp = board[pos1.y][pos1.x];
    board[pos1.y][pos1.x] = board[pos2.y][pos2.x];
    board[pos2.y][pos2.x] = tmp;

    check();
    return true;
}

bool Board::replace(Point pos) {
    if (!checkAlign(pos)) return false;
    static int dx[] = { 0, 1, 0, 1 };
    static int dy[] = { 0, 0, 1, 1 };
    int color = board[pos.y][pos.x];
    for (int i = 1; i < 4; ++i) {
        if (!checkAlign(Point(pos.x + dx[i], pos.y + dy[i]))) return false;
        if (board[pos.y + dy[i]][pos.x + dx[i]] != color) return false;
    }
    auto newTile = getNextTile(seed, score + turn);
    for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < 2; ++x) {
            board[pos.y + y][pos.x + x] = newTile[x + y * 2];
        }
    }
    ++score;
    check();
    return true;
}

void Board::check() {
    static int dx[] = { 0, 1, 0, 1 };
    static int dy[] = { 0, 0, 1, 1 };
    for (int y = 0; y < BOARD_SIZE - 1; ++y) {
        for (int x = 0; x < BOARD_SIZE - 1; ++x) {
            int color = board[y][x];
            int cnt = 0;
            for (int i = 1; i < 4; ++i) {
                if (!checkAlign(Point(x + dx[i], y + dy[i]))) break;
                if (board[y + dy[i]][x + dx[i]] != color) break;
                else ++cnt;
            }
            if (cnt == 3) replace(Point(x, y));
        }
    }
}

int Board::getCell(Point pos) {
    if (!checkAlign(pos)) return -1;
    return board[pos.y][pos.x];
}

int Board::getTurn() {
    return turn;
}

int Board::getScore() {
    return score;
}

bool Board::canSwap() {
    return (turn < MAX_TURN);
}

void Board::printBoard(int printType) {
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            printCell(board[y][x], printType);
        }
        cout << endl;
    }
}

vector<vector<int>> Board::getBoard() {
    return board;
}

Board::~Board() {
    vector<vector<int>> tmp;
    board.swap(tmp);
}

unsigned XORShift::random() {
    unsigned t;
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

bool checkAlign(Point pos) {
    if (pos.x < 0 || pos.x >= BOARD_SIZE) return false;
    if (pos.y < 0 || pos.y >= BOARD_SIZE) return false;
    return true;
}

vector<vector<int>> makeBoard(int seed) {
    XORShift rand(seed);
    vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE));
    int dx[] = { -1, -1,  0, 0 };
    int dy[] = { -1,  0, -1, 0 };
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int cnt, cell;
            do {
                cnt = 0;
                cell = std::abs((int)rand()) % COLOR;
                for (int i = 0; i < 3; ++i) {
                    if (checkAlign(Point(x + dx[i], y + dy[i]))) {
                        if (cell == board[y + dy[i]][x + dx[i]]) ++cnt;
                    }
                }
            } while (cnt == 3);
            board[y][x] = cell;
        }
    }
    return board;
}

vector<int> getNextTile(unsigned int seed, unsigned int turn) {
    vector<int> ret(4);
    const unsigned int offset = 1234;
    XORShift rand(seed + turn * turn + offset);
    for (int i = 0; i < 3; ++i) {
        ret[i] = std::abs((int)rand()) % COLOR;
    }
    unsigned int cnt, cell;
    do {
        cnt = 0;
        cell = std::abs((int)rand()) % COLOR;
        for (int i = 0; i < 3; ++i) if (cell == ret[i]) ++cnt;
    } while (cnt == 3);
    ret[3] = cell;
    return ret;
}

void printTile(vector<int> &tile, int printType) {
    for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < 2; ++x) {
            printCell(tile[x + y * 2], printType);
        }
        cout << endl;
    }
}

void printCell(int cell, int printType)
{
    switch (printType) {
        case COLOR_NUMBER:
            cout << (int)cell;
            return;
        case COLOR_STRING:
            cout << colorChar[cell];
            return;
    }

    switch (cell) {
        case RED:
            cout << "\033[" << FG_LIGHT_RED << "m";
            break;
        case YELLOW:
            cout << "\033[" << FG_LIGHT_YELLOW << "m";
            break;
        case GREEN:
            cout << "\033[" << FG_LIGHT_GREEN << "m";
            break;
        case BLUR:
            cout << "\033[" << FG_LIGHT_BLUE << "m";
            break;
        default:
            cout << "\033[39m";
            break;
    }
    cout << "#" << "\033[39m";
}
// ***********************************************************************

int main()
{
    int seed = 0;
    Board board(seed);

    int dx[] = { 1, 0};
    int dy[] = { 0, 1};

    board.printBoard(); // for debug
    cout << endl;       // for debug

    // 左上から交換可能な位置を全探索し、最初に見つけたスコアが向上する場所に置く
    // * あえて計算量やメモリ効率等を悪くしています
    //   みんな改善してね！の意
    // もしスコアが上がらない場合はランダムに交換する
    Board *newBoard;
    while (board.canSwap()) {
        bool flag = false;
        for (int y = 0; !flag && y < BOARD_SIZE - 1; ++y) {
            for (int x = 0; !flag && x < BOARD_SIZE - 1; ++x) {
                for (int i = 0; i < 2; ++i) {
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    if (checkAlign(Point(nx, ny))) {
                        newBoard = new Board(seed, board.getTurn(), board.getScore(), board.getBoard());
                        newBoard->swap(Point(x, y), Point(nx, ny));
                        if (newBoard->getScore() > board.getScore()) {
                            flag = true;
                            board.swap(Point(x, y), Point(nx, ny));
                            cout << x << " " << y << " " << nx << " " << ny << endl;
                        }
                        delete newBoard;
                    }
                }
            }
        }
        if (!flag) {
            int x = rand() % (BOARD_SIZE - 1);
            int y = rand() % (BOARD_SIZE);
            board.swap(Point(x, y), Point(x + 1, y));
            cout << x << " " << y << " " << x + 1 << " " << y << endl;
        }
    }

    board.printBoard(); // for debug
    cout << endl;       // for debug
    cout << board.getScore() << endl;
    return 0;
}
