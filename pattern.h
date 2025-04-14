#ifndef PATTERN_H
#define PATTERN_H
#include <string>
#include <vector>
/**
 * @brief 棋型与对应的分数组成的模式
 */
class Pattern{
public:
    std::string pattern;
    int score;
    Pattern(std::string p,int s);
};

extern std::vector<Pattern> patterns;
extern std::vector<std::string> patternStrings;

#endif // PATTERN_H
