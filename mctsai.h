#ifndef MCTSAI_H
#define MCTSAI_H
#include <memory>
#include <vector>
#include <random>
#include <utility>
#include <stack>
#include <set>
#include "gamemodel.h"
#include "zobristhash.h"
const int MCTS_SIMULATION_COUNT = 10000; // MCTS模拟次数
const double MCTS_TIME_LIMIT = 5; // MCTS时间限制（秒）
const double EVAL_UCB_WEIGHT=0.618;//w是权重因子，范围[0,1]，控制评估分数对UCT的影响程度
const double NORMALIZE_LAMBDA=0.000618;//归一化缩放系数
const double EXPLORATION_WEIGHT=2;//UCB探索系数
// MCTS 树的节点类
class MCTSAI;
class MCTSNode:public std::enable_shared_from_this<MCTSNode> {
private:
    /**
     * @brief traverse 该函数仅为MCTSNode::getPossibleMoves()服务
     * @param s
     * @param p
     * @param i
     * @param sign
     */
    void traverse(std::set<std::pair<int,int>>&s,Point&p,int i,int sign);
public:
    // 在当前状态下，谁该下棋
    bool nextColor;

    //该节点被访问的次数
    int visits=0;

    //该节点获胜的次数，这里之所以是double，是因为算法中需要计算平均获胜次数，wins/visits可能为小数
    double wins=0;

    //这是该节点所代表棋局的评估分数，评估分数将结合UCB公式与alpha-beta评估函数所给出的分数
    double enhenced_ucb_val=-DOUBLE_INF;

    //父节点指针
    std::shared_ptr<MCTSNode> parent;

    //子节点集合
    std::vector<std::shared_ptr<MCTSNode>> children;

    //棋步的副本，代表当前棋局的状态
    std::vector<Point>state;

    //该节点属于哪一个AI管理，因为节点的某些操作需要访问MCTSAI中的函数
    std::shared_ptr<MCTSAI>mctsai_ptr;

    // Constructor for child nodes
    MCTSNode(std::vector<Point>theState,std::shared_ptr<MCTSNode>theParent,bool theNextColor,std::shared_ptr<MCTSAI>mctsai):
        state(std::move(theState)),
        parent(theParent),
        nextColor(theNextColor),
        mctsai_ptr(mctsai)
    {}

    /**
     * @brief getPossibleMoves 获得当前状态下所有可能的下一个动作
     * @return
     */
    std::vector<Point>getPossibleMoves();

    /**
     * @brief getPossibleMovesWithEvaluation 启发式的获取所有下一个可能的动作，会按照可能动作的价值来进行排序并返回一个栈，栈顶的就是价值更高的位置
     * @return
     */
    std::stack<Point> getPossibleMovesWithEvaluation();

    /**
     * @brief expandNodes 根据当前的棋盘状态，扩展自己的子节点，也就是将所有可能的下一个动作加到自己的子节点上
     */
    void expandNodes();

    /**
     * @brief calculateValue 这个函数将计算这个节点的价值，将结合UCB公式和alpha-beta的评估函数
     * @return 不但会返回计算结果，还会将计算结果存储到成员enhenced_ucb_val中
     */
    double calculateValue();

    /**
     * @brief getBestChild 这个函数仅在迭代的时候使用，最终选择将由另一个函数承担。
     * 从该节点返回分数最高的子节点，即val值最高的子节点，返回其原始指针，如果有多个分值最高的，只会返回第一个
     * @return 有可能返回nullptr，这种情况只会因为此时节点没有任何child
     */
    std::shared_ptr<MCTSNode> getBestChild();

    std::shared_ptr<MCTSNode> getFinalBestChild();
};

class MCTSAI:public std::enable_shared_from_this<MCTSAI> {
    friend class MCTSNode;
private:
    std::mt19937 rng;
    int simulationCount;//默认迭代一千次
    double timeLimit; //默认最大时间是5秒
    std::shared_ptr<GameModel> gameModel_ptr;
    std::unique_ptr<ZobristHash>zobristHash_ptr;
    std::shared_ptr<MCTSNode>root;
public:
    MCTSAI(const std::shared_ptr<GameModel>& theGameModel ,int simulations = 1000, double timeLimitSec = 5.0);
    std::pair<int, int> getBestMove();
private:

    /**
     * @brief evaluateState 站在nextColor的角度评判当前棋局的分数，比如nextColor代表黑色，则评价当前棋局中黑子的分数，越高越有利，如果是负数那就是不利
     * 如果nextColor是白色，那就是站在白色棋子的角度评价的，也是越高越有利，如果是负数，那就是对白色不利
     * @param state 当前棋局的状态
     * @param nextcolor
     * @return
     */
    int evaluateState(std::vector<Point> &state, bool nextcolor);
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
     * @param g
     * @return
     */
    int getScore(bool color, int x, int y,const std::vector<std::vector<int>>&g);

    /**
     * @brief mctsIterationLoop
     * @param newRoot
     * @return
     */
    std::pair<Point, std::shared_ptr<MCTSNode>> mctsIterationLoop();

    /**
     * @brief mctsSelection
     * @param node
     * @return
     */
    std::pair<std::shared_ptr<MCTSNode>, bool> mctsSelection(std::shared_ptr<MCTSNode>node);

    /**
     * @brief mctsExpansion
     * @param node
     */
    void mctsExpansion(std::shared_ptr<MCTSNode>node);

    /**
     * @brief mctsSimulation
     * @param node
     * @return
     */
    std::shared_ptr<MCTSNode>mctsSimulation(std::shared_ptr<MCTSNode>node);

    /**
     * @brief mctsBackPropagation
     * @param endNode
     * @param node
     */
    void mctsBackPropagation(std::shared_ptr<MCTSNode>endNode,std::shared_ptr<MCTSNode>node);

};

#endif // MCTSAI_H
