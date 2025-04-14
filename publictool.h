#ifndef PUBLICTOOL_H
#define PUBLICTOOL_H
#include"point.h"
#include<vector>
class PublicTool
{
public:
    PublicTool();
    static int randomIndex(int begin, int end);
    static bool posIsValid(int row, int col);
    static std::vector<std::vector<int>> generateGrid(const std::vector<Point>& state);
    /**
     * @brief checkWin
     * @param board
     * @param row
     * @param col
     * @return
     */
    static bool checkWin(const std::vector<std::vector<int>>& board, int row, int col);
    static bool checkWin(const std::vector<Point>&state);
};

#endif // PUBLICTOOL_H
