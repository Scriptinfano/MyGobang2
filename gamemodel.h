#ifndef GAMEMODEL_H
#define GAMEMODEL_H
#include <vector>
#include <memory>
#include "point.h"
#include "acstring.h"
/**
 * @brief 游戏的类型，是双人还是AI
 */
enum GameType
{
    MAN, // 双人模式
    AI,  // 人机对战模式（简单评分算法）
};
/**
 * @brief 游戏的状态
 */
enum GameStatus
{
    PLAYING,
    WIN,
    DEAD
};
/**
 * @brief 采用哪一个AI算法进行游戏
 */
enum AIType
{
    LOCALEVALUATION,
    MCTS,
    ALPHABETA
};
/**
 * @brief The Piece enum 某个位置的颜色，或者标识没有子
 */
enum Piece
{
    EMPTY,
    BLACK,
    WHITE
};

class GameModel
{
private:
    /**
     * @brief 标识下棋方，true表示黑棋方，false表示白旗方
     */
    bool playerFlag;
    /**
     * @brief 游戏状态
     */
    GameStatus gameStatus;
    /**
     * @brief AI算法的类型
     */
    AIType aiType;
    /**
     * @brief 游戏模式，人机对战还是双人对战
     */
    GameType gameType;

    bool checkDirection(int row, int col, int dx, int dy);

public:
    /**
     *
     * @brief 存储当前游戏棋盘和棋子的情况，空白为0，黑子为1，白子为-1，所有算法共用这一个棋盘
     * 由于其他类也要访问这个资源，所以这里用shared_ptr包装一下
     */
    std::shared_ptr<std::vector<std::vector<int>>> gameMapVec;

    GameModel(const AIType &aitype, const GameType gametype);

    /**
     * @brief steps 棋步数组，记录从开局到结束的每一步
     * 之所以开放给外部使用，是因为外界的AI需要访问这些数据，并对这些数据进行修改
     */
    std::vector<Point> steps;

    std::unique_ptr<AcString> acMachine;

    const std::shared_ptr<const std::vector<std::vector<int>>> getGameBoard() const
    {
        return gameMapVec;
    }
    /**
     * @brief checkBoardPos 检查棋盘上的某个位置上是什么子，还是根本没有子
     * @param row 横坐标
     * @param col 纵坐标
     * @return 枚举量，定义在文件上面
     */
    Piece checkBoardPos(int row, int col);

    /**
     * @brief startGame 按照指定的算法开始游戏
     */
    void startGame();

    /**
     * @brief updateGameMap 每次落子后更新游戏棋盘
     * @param row
     * @param col
     */
    void updateGameModel(int row, int col);

    /**
     * @brief isWin 判断游戏是否胜利，原理是落子的时候以当前子为中心根据横竖斜几个方向遍历
     * @param row
     * @param col
     * @return true表示
     */
    bool isWin(int row, int col);

    /**
     * @brief isDeadGame 判断是否和棋
     * @return
     */
    bool isDeadGame();

    /**
     * @brief getBoardValue 直接获取内部二维数组的值
     * @param row
     * @param col
     * @return
     */
    int getBoardValue(int row, int col);
    void setGameType(GameType newGameType);
    GameType getGameType() const;
    void setAiType(AIType newAiType);
    AIType getAiType() const;
    bool getPlayerFlag() const;
    GameStatus getGameStatus() const;
};

#endif // GAMEMODEL_H
