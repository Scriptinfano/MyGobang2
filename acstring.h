#ifndef ACSTRING_H
#define ACSTRING_H
#include<vector>
#include<unordered_map>

/**
 * @brief The AcNode class Trie树的节点
 */
class AcNode{
public:
    AcNode(int p, char c): parent(p), ch(c){}
    char ch;//该节点所代表的字符
    std::unordered_map<char, int> sons;//char代表trie树节点中存放的字符，int代表该节点在nodes数组中的索引
    int fail=-1;//fail指针，值是应指向的节点在nodes数组中的索引
    std::vector<int> output;//存储已经匹配到的模式串在数组paterns中的编号
    int parent;//父节点在nodes数组中的编号
};

class AcString
{
public:
    /**
     * @brief AcString 构造函数
     * @param patterns 传入所有模式串
     */
    AcString(const std::vector<std::string>&patterns);
    /**
     * @brief match
     * @param text 待匹配的字符串
     * @return 返回已经匹配的模式串在patterns数组中的索引
     */
    std::vector<int>match(const std::string &text);

private:
    int maxState=0;//记录Trie树的状态数，也就是节点数量的索引，作用是在构建Trie树的时候，每个新增的ACNode都有唯一的索引编号
    std::vector<AcNode> nodes;//Trie树节点集合
    std::vector<std::string> patterns;//模式串集合
    /**
     * @brief reloadPatterns 重新载入模式串
     * @param patterns 模式串集合
     */
    void loadPatterns(const std::vector<std::string>&patterns);

    /**
     * @brief AddState
     * @param parent 新节点的父节点在nodes数组中的编号是多少
     * @param ch 新节点保存的字符是什么
     */
    void addNodes(int parent, char ch);

    /**
     * @brief BuildGotoTable 构建trie树
     */
    void buildTrieTree();
    /**
     * @brief BuildFailTable 构建fail指针
     */
    void buildFailPointer();
};

#endif // ACSTRING_H
