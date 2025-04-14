#ifndef LOCALEVALUATIONAI_H
#define LOCALEVALUATIONAI_H
#include <vector>
#include <memory>

#include "gamemodel.h"
class LocalEvaluationAI
{
public:
    LocalEvaluationAI(const std::shared_ptr<GameModel>& theGameModel);

    Point getNextStep();

private:
    void calculateScore();

    /**
     * @brief 存储各个点位的评分情况，作为AI下棋的依据
     */
    std::vector<std::vector<int>> scoreMapVec;
    /**
     * @brief 访问棋盘数据的指针
     */
    std::shared_ptr<GameModel> gameModel;
};

#endif // LOCALEVALUATIONAI_H
