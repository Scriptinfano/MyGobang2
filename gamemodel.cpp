#include "gamemodel.h"
#include "publictool.h"
#include "globals.h"
#include "pattern.h"
GameModel::GameModel(const AIType &aitype, const AIType &otherAiType, const GameType gametype) : gameMapVec(std::make_shared<std::vector<std::vector<int>>>(BOARD_GRID_SIZE, std::vector<int>(BOARD_GRID_SIZE, 0))),
                                                                                                 aiType(aitype),
                                                                                                 gameType(gametype),
                                                                                                 playerFlag(true),
                                                                                                 gameStatus(DEAD),
                                                                                                 otherAiType(otherAiType)
{
    acMachine=std::make_unique<AcString>(patternStrings);
}

AIType GameModel::getAiType() const
{
    return aiType;
}

AIType GameModel::getOtherAiType() const
{
    return otherAiType;
}

void GameModel::setAiType(AIType newAiType)
{
    aiType = newAiType;
}

GameType GameModel::getGameType() const
{
    return gameType;
}

void GameModel::setGameType(GameType newGameType)
{
    gameType = newGameType;
}

Piece GameModel::checkBoardPos(int row,int col){
    if(!PublicTool::posIsValid(row,col)){
        throw std::out_of_range("pos不合法");
    }
    switch(gameMapVec->at(row).at(col)){
    case -1:
        return WHITE;
    case 0:
        return EMPTY;
    case 1:
        return BLACK;
    default:
        throw std::runtime_error("");
    }
}
void GameModel::startGame()
{
    gameMapVec->clear();
    gameMapVec->resize(BOARD_GRID_SIZE, std::vector<int>(BOARD_GRID_SIZE, 0));  // 直接创建 15x15 的 0 矩阵
    steps.clear();
    playerFlag=true;
    gameStatus=PLAYING;
}

void GameModel::updateGameModel(int row, int col)
{
    if (playerFlag){
        //playerFlag为true代表是黑棋在下
        gameMapVec->at(row).at(col) = 1;
        //记录棋步
        steps.emplace_back(true,row,col);
    }
    else{
        gameMapVec->at(row).at(col) = -1;
        steps.emplace_back(false,row,col);
    }
    playerFlag = !playerFlag;
}

bool GameModel::getPlayerFlag() const
{
    return playerFlag;
}

GameStatus GameModel::getGameStatus() const
{
    return gameStatus;
}

bool GameModel::checkDirection(int row, int col, int dx, int dy) {
    // 检查当前方向是否有五子连珠
    int count = 0;
    int color = gameMapVec->at(row).at(col);  // 当前棋子颜色

    // 检查一个方向上的连续五个棋子
    for (int i = -4; i <= 0; i++) {
        bool valid = true;
        for (int j = 0; j < 5; j++) {
            int r = row + (i + j) * dx;
            int c = col + (i + j) * dy;

            // 检查是否越界或棋子颜色不一致
            if (r < 0 || r >= BOARD_GRID_SIZE || c < 0 || c >= BOARD_GRID_SIZE || gameMapVec->at(r).at(c) != color) {
                valid = false;
                break;
            }
        }
        if (valid) {
            return true;
        }
    }
    return false;
}

bool GameModel::isWin(int row, int col) {
    // 横向、纵向、对角线（两个方向）四个方向的判断
    // 横向：dx=0, dy=1
    if (checkDirection(row, col, 0, 1)||checkDirection(row, col, 1, 0)||checkDirection(row, col, 1, 1)||checkDirection(row, col, -1, 1)){
        return true;
    }
    return false;
}


bool GameModel::isDeadGame()
{
    return steps.size()==BOARD_GRID_SIZE*BOARD_GRID_SIZE;
}

int GameModel::getBoardValue(int row, int col)
{
    return gameMapVec->at(row).at(col);
}

void GameModel::setGameStatus(GameStatus gst){
    gameStatus=gst;
}
