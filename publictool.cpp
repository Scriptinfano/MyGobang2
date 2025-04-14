#include "publictool.h"
#include <random>
#include "globals.h"

PublicTool::PublicTool() {}

int PublicTool::randomIndex(int begin, int end) {
    std::random_device rd;  // 获取真正随机数（用于种子）
    std::mt19937 gen(rd());  // 以随机设备作为种子，创建Mersenne Twister 19937 生成器
    std::uniform_int_distribution<int> dist(begin, end - 1);  // 均匀分布
    return dist(gen);  // 生成随机索引
}
bool PublicTool::posIsValid(int row, int col)
{
    return row >= 0 && row < BOARD_GRID_SIZE && col >= 0 && col < BOARD_GRID_SIZE;
}
std::vector<std::vector<int>> PublicTool::generateGrid(const std::vector<Point>& state) {
    std::vector<std::vector<int>> g(BOARD_GRID_SIZE, std::vector<int>(BOARD_GRID_SIZE, 0));
    for (const auto& i : state) {
        g[i.x][i.y] = i.color ? 1 : -1;
    }
    return std::move(g);
}

// Check if a move leads to a win
bool PublicTool::checkWin(const std::vector<std::vector<int>>& board, int row, int col) {
    int player = board[row][col];

    // Check horizontally
    for (int i = 0; i < 5; i++) {
        if (col - i > 0 && col - i + 4 < BOARD_GRID_SIZE &&
            board[row][col - i] == player &&
            board[row][col - i + 1] == player &&
            board[row][col - i + 2] == player &&
            board[row][col - i + 3] == player &&
            board[row][col - i + 4] == player)
            return true;
    }

    // Check vertically
    for (int i = 0; i < 5; i++) {
        if (row - i > 0 && row - i + 4 < BOARD_GRID_SIZE &&
            board[row - i][col] == player &&
            board[row - i + 1][col] == player &&
            board[row - i + 2][col] == player &&
            board[row - i + 3][col] == player &&
            board[row - i + 4][col] == player)
            return true;
    }

    // Check diagonal (top-left to bottom-right)
    for (int i = 0; i < 5; i++) {
        if (row - i > 0 && col - i > 0 && row - i + 4 < BOARD_GRID_SIZE && col - i + 4 < BOARD_GRID_SIZE &&
            board[row - i][col - i] == player &&
            board[row - i + 1][col - i + 1] == player &&
            board[row - i + 2][col - i + 2] == player &&
            board[row - i + 3][col - i + 3] == player &&
            board[row - i + 4][col - i + 4] == player)
            return true;
    }

    // Check diagonal (bottom-left to top-right)
    for (int i = 0; i < 5; i++) {
        if (row + i < BOARD_GRID_SIZE && col - i > 0 && row + i - 4 >= 0 && col - i + 4 < BOARD_GRID_SIZE &&
            board[row + i][col - i] == player &&
            board[row + i - 1][col - i + 1] == player &&
            board[row + i - 2][col - i + 2] == player &&
            board[row + i - 3][col - i + 3] == player &&
            board[row + i - 4][col - i + 4] == player)
            return true;
    }

    return false;
}
/**
 * @brief checkDirection 这个函数只给PublicTool::checkWin(const std::vector<Point> &state)服务，不要将声明放在其他头文件中
 * @param board
 * @param x
 * @param y
 * @param dx
 * @param dy
 * @return
 */
bool checkDirection(int board[BOARD_GRID_SIZE][BOARD_GRID_SIZE], int x, int y, int dx, int dy) {
    int player = board[x][y];
    if (player == 0) return false; // 当前位置无棋子

    for (int i = 1; i < 5; ++i) {
        int nx = x + i * dx;
        int ny = y + i * dy;
        if (nx < 0 || nx >= BOARD_GRID_SIZE || ny < 0 || ny >= BOARD_GRID_SIZE || board[nx][ny] != player)
            return false;
    }
    return true; // 找到了五连
}
bool PublicTool::checkWin(const std::vector<Point> &state)
{
    int board[BOARD_GRID_SIZE][BOARD_GRID_SIZE]={};//自动初始化为全0状态
    for(auto s:state){
        board[s.x][s.y]=s.color?1:-1;
    }

    for (int x = 0; x < BOARD_GRID_SIZE; ++x) {
        for (int y = 0; y < BOARD_GRID_SIZE; ++y) {
            if (board[x][y] != 0) { // 只检查有棋子的位置
                if (checkDirection(board, x, y, 1, 0) ||  // 水平方向
                    checkDirection(board, x, y, 0, 1) ||  // 垂直方向
                    checkDirection(board, x, y, 1, 1) ||  // 左上到右下对角线
                    checkDirection(board, x, y, 1, -1))   // 右上到左下对角线
                {
                    return true; // 找到五连
                }
            }
        }
    }
    return false; // 没有找到五连
}
