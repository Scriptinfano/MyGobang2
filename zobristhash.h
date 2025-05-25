#ifndef ZOBRISTHASH_H
#define ZOBRISTHASH_H
#include<unordered_map>
#include "point.h"
#include <vector>
#include "globals.h"
using ll=long long;
/**
 * @brief The ZobristHash class
 *
Zorbrist哈希的操作
假设当前棋盘状态的哈希值是 hash，我们在 (x, y) 位置 落一个黑子，执行： hash ^= zobrist_black[x][y];
zobrist_black[x][y] 是一个预先生成的 随机数，它唯一地代表 (x, y) 位置的黑子状态。
XOR 操作相当于 把这个状态加入哈希值中
当我们 撤销刚才的落子，再次执行：hash ^= zobrist_black[x][y];
 */
class ZobristHash
{

private:
    //黑、白棋的zobrist表，内存64bit随机数，这个随机数在后面是不会变化的
    ll zobristBlack[BOARD_GRID_SIZE][BOARD_GRID_SIZE],zobristWhite[BOARD_GRID_SIZE][BOARD_GRID_SIZE];
    //存储每一个棋局状态所代表的hash值，int代表该局面对应的分数
    std::unordered_map<ll,int>zobristList;

public:
    ZobristHash();
    /**
     * @brief getZobristHash 获取当前棋盘状态的哈希值
     * @param state 当前棋盘状态
     * @return 返回当前棋盘状态的哈希值
     */
    ll getZobristHash(std::vector<Point>state);
    bool hasKey(ll key);
    /**
     * @brief getHashValue 这是一个带有输出型参数的函数，输出型参数会携带出hashValue，前提是它的key存在
     * @param key 该哈希的键是什么
     * @param color 站在哪一方的颜色取出hash值，因为两方的哈希值互为相反数
     * @param res 最终的结果
     * @return 如果返回true则可以顺利取出res的结果，如果返回false，则不能采用res的结果
     */
    bool getHashValue(ll key,bool color,int &res);
    /**
     * @brief updateHashValue 更新哈希值
     * @param key 哈希的键
     * @param value 哈希值
     */
    void updateHashValue(ll key,int value);

    //以下两个函数是用来保存和加载zobrist表的，每次对局保存Zobrist表，保存的对局越多，那么全局评估就越来越没有必要，所以需要保存到文件中，下次启动时加载
    /**
     * @brief saveZobristHash 保存zobrist表到文件中
     */
    void saveZobristHash();
    /**
     * @brief loadZobristHash 从文件中加载zobrist表
     */
    void loadZobristHash();
private:
    /**
     * @brief getRandom64 生成一个64位的随机数
     * @return 返回一个64位的随机数
     */
    ll getRandom64();

    /**
     * @brief getDataFilePath 获取zobrist表的文件路径
     * @return 返回zobrist表的文件路径
     */
    std::string getDataFilePath();
};

#endif // ZOBRISTHASH_H
