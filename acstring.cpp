#include "acstring.h"
#include <QDebug>
AcString::AcString(const std::vector<std::string> &patterns)
{
    addNodes(-1, 'r'); // 创建根节点
    nodes[0].fail = -1;
    loadPatterns(patterns); // 装载模式串
    buildTrieTree();        // 构建Trie树
    buildFailPointer();     // 构建fail指针
}

std::vector<int> AcString::match(const std::string &text)
{
    std::vector<int> result;
    // qDebug()<<"进入match函数，被匹配的字符串是"<<text;
    int currentIndex = 0;

    unsigned int i;
    std::unordered_map<char, int>::iterator tmpIter;
    for (i = 0; i < text.size();)
    {

        if ((tmpIter = nodes[currentIndex].sons.find(text[i])) != nodes[currentIndex].sons.end())
        {
            currentIndex = tmpIter->second;
            i++;
        }
        else
        {
            while (nodes[currentIndex].fail != -1 && nodes[currentIndex].sons.find(text[i]) == nodes[currentIndex].sons.end())
            {
                currentIndex = nodes[currentIndex].fail;
            }
            if (nodes[currentIndex].sons.find(text[i]) == nodes[currentIndex].sons.end())
            {
                i++;
            }
        }
        if (nodes[currentIndex].output.size())
        {
            result.insert(result.end(), nodes[currentIndex].output.begin(), nodes[currentIndex].output.end());
        }
    }

    return result;
}
/**
 * @brief loadPatterns 载入模式串
 * @param patterns 模式串集合
 */
void AcString::loadPatterns(const std::vector<std::string> &patterns)
{
    this->patterns = patterns;
}

/**
 * @brief AddState
 * @param parent 新节点的父节点在nodes数组中的编号是多少
 * @param ch 新节点保存的字符是什么
 */
void AcString::addNodes(int parent, char ch)
{
    nodes.push_back(AcNode(parent, ch));
}

/**
 * @brief BuildGotoTable 构建trie树
 */
void AcString::buildTrieTree()
{
    for (int i = 0; i < patterns.size(); i++)
    {
        int curindex = 0;
        for (int j = 0; j < patterns[i].size(); j++)
        {
            if (nodes[curindex].sons.find(patterns[i][j]) == nodes[curindex].sons.end())
            {
                nodes[curindex].sons[patterns[i][j]] = ++maxState; // maxState会先变化到1,然后再赋值，因为第一个状态是根节点，而这里开始的时候已经是第二个节点了
                addNodes(curindex, patterns[i][j]);
                curindex = maxState;
            }
            else
            {
                curindex = nodes[curindex].sons[patterns[i][j]];
            }
        }
        nodes[curindex].output.push_back(i);
    }
}
/**
 * @brief BuildFailTable 构建fail指针
 */
void AcString::buildFailPointer()
{
    std::vector<int> midNodesIndex;
    AcNode root = nodes[0];
    std::unordered_map<char, int>::iterator iter1, iter2;
    for (iter1 = root.sons.begin(); iter1 != root.sons.end(); iter1++)
    {
        nodes[iter1->second].fail = 0;
        AcNode &currentNode = nodes[iter1->second];
        for (iter2 = currentNode.sons.begin(); iter2 != currentNode.sons.end(); iter2++)
        {
            midNodesIndex.push_back(iter2->second);
        }
    }
    // BFS遍历Tire树过程
    while (midNodesIndex.size())
    {
        std::vector<int> newMidNodesIndex;

        unsigned int i;
        for (i = 0; i < midNodesIndex.size(); i++)
        {
            AcNode &currentNode = nodes[midNodesIndex[i]];

            int currentFail = nodes[currentNode.parent].fail;
            while (true)
            {
                AcNode &currentFailNode = nodes[currentFail];

                if (currentFailNode.sons.find(currentNode.ch) != currentFailNode.sons.end())
                {

                    currentNode.fail = currentFailNode.sons.find(currentNode.ch)->second;

                    if (nodes[currentNode.fail].output.size())
                    {
                        currentNode.output.insert(currentNode.output.end(), nodes[currentNode.fail].output.begin(), nodes[currentNode.fail].output.end());
                    }

                    break;
                }
                else
                {
                    currentFail = currentFailNode.fail;
                }

                if (currentFail == -1)
                {
                    currentNode.fail = 0;
                    break;
                }
            }

            for (iter1 = currentNode.sons.begin(); iter1 != currentNode.sons.end(); iter1++)
            {

                newMidNodesIndex.push_back(iter1->second);
            }
        }
        midNodesIndex = newMidNodesIndex;
    }
}
