#ifndef ALPHABETAAI_H
#define ALPHABETAAI_H


#include <stack>
#include "gamemodel.h"
#include "zobristhash.h"
class AlphaBetaAI {
private:
     //AI访问数据层的接口
    std::shared_ptr<GameModel> gameModel;
    std::unique_ptr<ZobristHash>zobristHash_ptr;
public:
    /**
     * @brief AlphaBetaAI
     */
    AlphaBetaAI(const std::shared_ptr<GameModel>& theGameModel);

    /**
     * @brief getBestMove 这是协调本算法所有流程的公开接口，返回经过算法运算之后的最佳棋步
     * @param state 传入棋步
     * @return 返回最佳落子位置
     */
    Point getBestMove();
private:
    /**
     * @brief alphabetaSearch 这是α-β剪枝的主流程函数，也是一个递归函数
     * @param state_ptr 指向棋步对象的指针
     * @param nowcolor 当前执棋方颜色
     * @param depth 当前递归深度
     * @param alpha 当前α值
     * @param beta 当前β值
     * @param maxDepth 最大递归深度
     * @return 返回经过算法计算的最佳落子点
     */
    Point alphabetaSearch(std::vector<Point>&state,bool nowcolor,int depth,int alpha,int beta, int maxDepth);
    /**
     * @brief evaluateState 站在nextcolor的角度评估当前局面的分数，如果是正数那就是对自己有利，如果是负数那就是对自己不利
     * @param state 待评估的局面
     * @param nextcolor 站在谁的角度评估指定的局面
     * @return 返回评估的分数
     */
    int evaluateState(std::vector<Point>&state,bool nextcolor);

    /**
     * @brief 获取当前棋局所有可能的下一步落子点，并按评估分数排序。
     *
     * 该函数遍历当前棋盘上所有的落子点，并检查其周围 8 个方向的空位作为可能的下一步落子点。
     * 对每个可能的落子点，模拟该点落子后计算棋局得分，并按照得分升序排序，最终返回一个按分数排序的候选点栈。
     *
     * @param state 当前棋局的所有落子点状态，包含棋子颜色与坐标。
     * @param nowcolor 当前棋手的颜色，true 代表黑子，false 代表白子。
     * @return std::stack<AiJudge::point> 按评估分数降序排列的候选落子点栈，栈顶是分数最大的候选点
     */
    std::stack<Point> getPossibleNextStep(std::vector<Point>& state, bool nowcolor);
    /**
     * @brief getScoreInDir
     * @param color
     * @param x
     * @param y
     * @param state
     * @param dir
     * @return
     */
    int getScoreInDir(bool color,int x,int y,std::vector<Point> state,int dir);
    /**
     * @brief getScore
     * @param color
     * @param x
     * @param y
     * @return
     */
    int getScore(bool color, int x, int y,const std::vector<std::vector<int>>&g);
};

#endif // ALPHABETAAI_H
