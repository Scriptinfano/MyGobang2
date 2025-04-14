#include "localevaluationai.h"
#include "globals.h"
#include "publictool.h"
#include <algorithm>
LocalEvaluationAI::LocalEvaluationAI(const std::shared_ptr<GameModel>& theGameModel)
    : scoreMapVec(BOARD_GRID_SIZE, std::vector<int>(BOARD_GRID_SIZE, 0)), // 直接在初始化列表中初始化
    gameModel(theGameModel)
{
}
void LocalEvaluationAI::calculateScore()
{
    int personNum = 0;
    int botNum = 0;
    int emptyNum = 0;
    //清空并初始化评分数组
    for (auto &row : scoreMapVec) {
        std::fill(row.begin(), row.end(), 0);
    }
    //遍历棋盘上的所有点，遇到空白点就计算周围的己方棋子数量和对方棋子数
    for (int x = 0; x < BOARD_GRID_SIZE; x++) {
        for (int y = 0; y < BOARD_GRID_SIZE; y++) {
            //对空白点做处理
            if (gameModel->checkBoardPos(x,y)==EMPTY) {
                //遍历空白点周围的八个方向
                for(int dir=0;dir<4;dir++){
                    personNum=0;
                    botNum=0;
                    emptyNum=0;
                    int sign=1;
                    for(int i=1;i<=4;i++){
                        int tx=x+sign*i*dx[dir],ty=y+sign*i*dy[dir];
                        if(PublicTool::posIsValid(tx,ty)){
                            if(gameModel->checkBoardPos(tx,ty)==BLACK)
                                personNum++;
                            else if(gameModel->checkBoardPos(tx,ty)==WHITE)
                                botNum++;
                        }else
                            break;
                    }
                    sign=-sign;
                    for(int i=1;i<=4;i++){
                        int tx=x+sign*i*dx[dir],ty=y+sign*i*dy[dir];
                        if(PublicTool::posIsValid(tx,ty)){
                            if(gameModel->checkBoardPos(tx,ty)==BLACK)
                                personNum++;
                            else if(gameModel->checkBoardPos(tx,ty)==WHITE)
                                botNum++;
                        }else
                            break;
                    }
                    //扫描完一个方向之后，累加这个方向的分数
                    // 累加方向评分
                    if (personNum == 1)
                        scoreMapVec[x][y] += 10;
                    else if (personNum == 2) {
                        if (emptyNum == 1)
                            scoreMapVec[x][y] += 25;  // 眠二
                        else if (emptyNum == 2)
                            scoreMapVec[x][y] += 50;  // 活二
                    } else if (personNum == 3) {
                        if (emptyNum == 1)
                            scoreMapVec[x][y] += 100;  // 眠三
                        else if (emptyNum == 2)
                            scoreMapVec[x][y] += 500;  // 活三
                    } else if (personNum == 4) {
                        if (emptyNum == 1)
                            scoreMapVec[x][y] += 2500; // 冲四
                        else if (emptyNum == 2)
                            scoreMapVec[x][y] += 5000; // 活四
                    } else if (personNum >= 5)
                        scoreMapVec[x][y] += 100000;  // 五连胜利

                    // 进攻部分
                    if (botNum == 1)
                        scoreMapVec[x][y] += 10;
                    else if (botNum == 2) {
                        if (emptyNum == 1)
                            scoreMapVec[x][y] += 25;
                        else if (emptyNum == 2)
                            scoreMapVec[x][y] += 50;
                    } else if (botNum == 3) {
                        if (emptyNum == 1)
                            scoreMapVec[x][y] += 100;
                        else if (emptyNum == 2)
                            scoreMapVec[x][y] += 500;
                    } else if (botNum == 4) {
                        if (emptyNum == 1)
                            scoreMapVec[x][y] += 2600; // AI冲四
                        else if (emptyNum == 2)
                            scoreMapVec[x][y] += 5100; // AI活四
                    } else if (botNum >= 5)
                        scoreMapVec[x][y] += 100000;
                }
            }
        }
    }
}
Point LocalEvaluationAI::getNextStep()
{
    calculateScore();
    // 从评分中找出最大分数的位置，然后随机选择一个
    int maxScore = 0;
    std::vector<std::pair<int, int>> maxPoints; //存储最大分数坐标的容器
    for (int row = 0; row < BOARD_GRID_SIZE; row++) {
        for (int col = 0; col < BOARD_GRID_SIZE; col++) {
            if (gameModel->checkBoardPos(row,col)==EMPTY) {
                if (scoreMapVec[row][col] > maxScore) {
                    maxPoints.clear();
                    maxScore = scoreMapVec[row][col];
                    maxPoints.push_back(std::make_pair(row, col));
                } else if (scoreMapVec[row][col] == maxScore) {
                    maxPoints.push_back(std::make_pair(row, col));
                }
            }
        }
    }
    int index = PublicTool::randomIndex(0, maxPoints.size());
    std::pair<int,int>p= maxPoints.at(index);
    Point temp(false,p.first,p.second);
    return std::move(temp);
}
