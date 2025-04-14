#include "alphabetaai.h"
#include <random>
#include "publictool.h"
#include "globals.h"
#include <cstring>
#include "pattern.h"
#include <map>
#include "pairhash.h"
#include <unordered_set>
#include<algorithm>
#include<QDebug>
AlphaBetaAI::AlphaBetaAI(const std::shared_ptr<GameModel>& theGameModel):
    gameModel(theGameModel)
{
    zobristHash_ptr=std::make_unique<ZobristHash>();
}

Point AlphaBetaAI::getBestMove(){
    //先看一下steps是不是只有一步，如果只有一步，那AI就在落子点周围的合法点中挑一个点下
    std::vector<Point>&state=gameModel->steps;
    if(state.size()==1){
        int x = state.back().x,y = state.back().y;
        //这种情况只会发生在人类先手的情况下，即人类拿的是黑棋，AI拿的是白棋
        std::vector<std::pair<int, int>> directions = {
            {1, 0}, {0, 1}, {-1, 0}, {0, -1},  // 右、下、左、上
            {1, 1}, {-1, -1}, {1, -1}, {-1, 1} // 右下、左上、右上、左下（斜向）
        };

        // 打乱方向顺序，增加随机性
        std::shuffle(directions.begin(), directions.end(), std::mt19937(std::random_device()()));

        for (auto [dx, dy] : directions) {
            int nx = x + dx, ny = y + dy;
            // 确保落子不会超出棋盘范围，并且该位置为空
            if (PublicTool::posIsValid(nx,ny) &&gameModel->checkBoardPos(nx,ny)==EMPTY) {
                return Point(0,nx,ny);
            }
        }
    }
    Point aiStep(0,0,0,-INF);
    for(int i=2;i<=MAXDEPTH;i+=2){
        Point tmpStep=alphabetaSearch(state,false,0,-INF,INF,i);
        //更新aiStep
        if(tmpStep.score>aiStep.score){
             aiStep=tmpStep;
             aiStep.x=tmpStep.x;
             aiStep.y=tmpStep.y;
        }
        //有必胜棋型的出现
        if(tmpStep.score>720*2){
            aiStep.x=tmpStep.x;
            aiStep.y=tmpStep.y;
            break;
        }
    }
    return aiStep;
}

Point AlphaBetaAI::alphabetaSearch(std::vector<Point> &state, bool nowcolor, int depth, int alpha, int beta, int maxDepth)
{
    if(depth==maxDepth){
        //到达底层之后返回
        Point nextStep=state[state.size()-maxDepth];
        nextStep.score=evaluateState(state,nextStep.color);
        return nextStep;
    }
    std::stack<Point>nextPossibleStep=getPossibleNextStep(state,nowcolor);
    //qDebug()<<"nextPossibleStep 最顶部元素所代表的分数是"<<nextPossibleStep.top().score;
    Point maxPoint(nowcolor,-1,-1,-INF),minPoint(nowcolor,-1,-1,INF);
    int cnt=0;
    while(!nextPossibleStep.empty()){
        cnt++;
        if(cnt>10&&maxDepth>4)
            break;
        Point tpstep=nextPossibleStep.top();
        nextPossibleStep.pop();
        state.push_back(tpstep);
        ll tkey=zobristHash_ptr->getZobristHash(state);
        if(zobristHash_ptr->hasKey(tkey)){
            int selfScore=evaluateState(state,nowcolor);
            int otherScore=evaluateState(state,!nowcolor);
            if(nowcolor)
                zobristHash_ptr->updateHashValue(tkey,selfScore);
            else
                zobristHash_ptr->updateHashValue(tkey,otherScore);
        }
        Point initPoint=alphabetaSearch(state,!nowcolor,depth+1,alpha,beta,maxDepth);
        state.pop_back();
        if(depth%2==0){
            if(initPoint.score>maxPoint.score)
                maxPoint=initPoint;
            alpha=std::max(alpha,maxPoint.score);
        }else{
            if(initPoint.score<minPoint.score)
                minPoint=initPoint;
            beta=std::min(beta,minPoint.score);
        }
        if(alpha>=beta)
            break;
    }
    if(depth%2==0)
        return maxPoint;
    else
        return minPoint;
}

int AlphaBetaAI::evaluateState(std::vector<Point> &state, bool nextcolor)
{
    ll nowkey=zobristHash_ptr->getZobristHash(state);
    int res;
    /*
    为什么要在取得hash值时指定颜色：
    保存哈希值的哈希表存的是以黑棋角度计算的分数（黑棋得分 - 白棋得分）。
    如果当前棋手是白棋（color = false），需要反转这个值，得到从白棋角度计算的分数
    这样的话，无论你是什么颜色，最后拿到一个值，只要是正的，就说明它对你有利，如果它是负的，那么它就对你有弊
    */
    bool valid=zobristHash_ptr->getHashValue(nowkey,nextcolor,res);
    if(valid)
        return res;
    /*
     * 定义一个之前的状态，回到对手落最后一个子之前的状态
     * 为什么要回到上一个状态：
     * befState 主要用于增量更新评估分数，从而提高 evaluateState 函数的效率。优化点在于利用前一个局面（befState）的缓存，而不是每次都进行全盘评估。
     * 如果 befState 之前已经计算过并缓存，我们就可以基于 befState 的分数，增量更新对方刚刚落子的影响，而不是重新计算整个棋盘。
     * 在评估分数时，需要考虑对方最后一步的影响，而 befState 提供了对方落子之前的基准分数，用来做增量更新。
     */
    std::vector<Point>befState;
    befState.assign(state.begin(),state.end()-1);
    //获取这个之前状态的hash值
    ll befKey=zobristHash_ptr->getZobristHash(befState);
    //取得对手落下的最后一个子
    Point nowPoint=state.back();
    bool nowColor=nextcolor?1:-1;
    //看一下之前局面有没有被缓存
    valid=zobristHash_ptr->getHashValue(befKey,nextcolor,res);
    if(valid){
        //之前的局面被缓存的情况，则增量更新分数
        int befval=res;
        int x=nowPoint.x,y=nowPoint.y;
        //在四个联通方向上，每个联通方向的单一方向扫描7个子，则每个联通方向就是14个子，四个联通方向最多就是4*14=56个子
        for(int dir=0;dir<4;dir++){
            int sign=-1;
            for(int i=1;i<8;i++){
                int tx=x+sign*i*dx[dir],ty=y+sign*i*dy[dir];
                if(!PublicTool::posIsValid(tx,ty)){
                    break;
                }
                int boardValue=gameModel->getBoardValue(tx,ty);
                if(boardValue==0)
                    continue;
                else if(boardValue==-nowColor){
                    /*
                    在扫描的过程中，遇到了对方棋子(tx,ty)
                    getScoreInDir(!color,tx,ty,befState,dir):对方棋子(tx,ty)在对方棋子还没落下时的某一方向的棋型得分
                    getScoreInDir(!color,tx,ty,state,dir):对方棋子(tx,ty)在对方棋子已经落下的某一方向的棋型得分
                    如果 state 让对方的棋型变强了（得分上升，getScoreInDir(!color,tx,ty,state,dir)>getScoreInDir(!color,tx,ty,befState,dir)），则 befval 会 减少（+ 旧 - 新）。
                    如果 state 让对方的棋型变弱了（得分下降，getScoreInDir(!color,tx,ty,state,dir)<getScoreInDir(!color,tx,ty,befState,dir)），则 befval 会 增加（+ 旧 - 新）。
                    */
                    befval=befval+getScoreInDir(!nextcolor,tx,ty,befState,dir)-getScoreInDir(!nextcolor,tx,ty,state,dir);
                }
                else if(boardValue==nowColor){
                    befval=befval-getScoreInDir(nextcolor,tx,ty,befState,dir)+getScoreInDir(nextcolor,tx,ty,state,dir);
                }
            }
            sign=-sign;
            for(int i=1;i<8;i++){
                int tx=x+sign*i*dx[dir],ty=y+sign*i*dy[dir];
                if(!PublicTool::posIsValid(tx,ty)){
                    break;
                }
                int boardValue=gameModel->getBoardValue(tx,ty);
                if(boardValue==0)
                    continue;
                else if(boardValue==-nowColor){
                    /*
                    在扫描的过程中，遇到了对方棋子(tx,ty)
                    getScoreInDir(!color,tx,ty,befState,dir):对方棋子(tx,ty)在对方棋子还没落下时的某一方向的棋型得分
                    getScoreInDir(!color,tx,ty,state,dir):对方棋子(tx,ty)在对方棋子已经落下的某一方向的棋型得分
                    如果 state 让对方的棋型变强了（得分上升，getScoreInDir(!color,tx,ty,state,dir)>getScoreInDir(!color,tx,ty,befState,dir)），则 befval 会 减少（+ 旧 - 新）。
                    如果 state 让对方的棋型变弱了（得分下降，getScoreInDir(!color,tx,ty,state,dir)<getScoreInDir(!color,tx,ty,befState,dir)），则 befval 会 增加（+ 旧 - 新）。
                    */
                    befval=befval+getScoreInDir(!nextcolor,tx,ty,befState,dir)-getScoreInDir(!nextcolor,tx,ty,state,dir);
                }
                else if(boardValue==nowColor){
                    befval=befval-getScoreInDir(nextcolor,tx,ty,befState,dir)+getScoreInDir(nextcolor,tx,ty,state,dir);
                }
            }
        }
        befval+=getScore(nextcolor,x,y,PublicTool::generateGrid(state));
        if(nextcolor)
            zobristHash_ptr->updateHashValue(nowkey,befval);
        else
            zobristHash_ptr->updateHashValue(nowkey,-befval);
        return befval;
    }
    //如果对手落最后一个子之前的状态没有被缓存，则只能全局评估
    int pointBLK=0,pointWIT=0;
    std::vector<std::vector<int>>board=PublicTool::generateGrid(state);
    for(auto point:state){
        if(point.color==BLACK)
            pointBLK+=getScore(true,point.x,point.y,board);
        else
            pointWIT+=getScore(false,point.x,point.y,board);
    }
    int val = 0;
    if (nextcolor){
        val = pointBLK- pointWIT;
        zobristHash_ptr->updateHashValue(nowkey,val);
    }
    else{
        val = pointWIT- pointBLK;
        zobristHash_ptr->updateHashValue(nowkey,-val);
    }
    //TODO 以下几行代码仅测试使用，测试出val在整个棋局中的一个变化的范围
    if(val>MAXVAL)
        MAXVAL=val;
    if(val<MINVAL)
        MINVAL=val;
    return val;
}

std::stack<Point> AlphaBetaAI::getPossibleNextStep(std::vector<Point> &state, bool nowcolor)
{
    //8个单向方向的偏移向量数组
    const int dtx[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };
    const int dty[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
    //pairSet用来检测某个坐标是否已经被扫描过了
    std::unordered_set<std::pair<int, int>, PairHash, PairEqual> pairSet;
    //scoreMap保存每个点的分数，键是分数，因为map会按照键的大小对值进行排序存放，后面要选出分数尽可能大的点
    //而且这里用multimap不仅可以根据键查到值，而且是允许重复的键的，即有可能哪两个分数相同
    std::multimap<int, std::pair<int, int>>scoreMap;
    //将已有点存入集合A
    for (auto point : state)
        pairSet.insert({ point.x,point.y });
    for (auto point : state) {
        //扫描每个已落子点周围的八个点
        for (int i = 0; i < 8; i++) {
            int tx = point.x + dtx[i], ty = point.y + dty[i];
            //扫描到一个非法位置，或者扫描到一个已经落子的位置则继续绕圈
            if (!PublicTool::posIsValid(tx,ty)||gameModel->checkBoardPos(tx,ty)!=EMPTY)
                continue;
            //之前这个合法空点已经被扫描过了
            if (pairSet.count({ tx, ty }))
                continue;
            //将未被扫描的合法点加入已扫描集合
            pairSet.insert({ tx, ty });
            //模拟在扫描到的点上落子
            state.push_back({ nowcolor, tx, ty });
            //让下一步的当前子的val尽可能的大
            //然后评估模拟落子之后的局面
            scoreMap.insert({ evaluateState(state, nowcolor), {tx ,ty} });
            state.pop_back();
        }
    }
    //将最后结果加入stack中（从小到大）
    std::stack<Point> ans;
    for (const auto& [key, value] : scoreMap) {
        int x = value.first, y = value.second;
        ans.push(Point(nowcolor,x,y,key));
    }
    return ans;
}

int AlphaBetaAI::getScoreInDir(bool color,int x,int y,std::vector<Point> state,int dir){
    std::vector<std::vector<int>>g=PublicTool::generateGrid(state);
    int ans = 0;
    int nowcolor = color ? 1 : -1;
    std::deque<char> acstringDeque;
    acstringDeque.push_back(1);
    int sign = -1;
    //获取该点dir方向的棋子排布
    //AC 自动机在匹配棋型时，会利用 2 来避免匹配到错误的活三、活四等棋型。
    //在 dir 方向上，以 (x, y) 为中心，向两边最多扩展 3 步，总共 7 个格子
    for (int i = 1; i < 8; ++i) {
        int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
        if (!PublicTool::posIsValid(tx, ty)){
            acstringDeque.push_front(2);
            break;
        }
        int boardValue=g.at(tx).at(ty);
        if(boardValue==0)
            acstringDeque.push_front('0');
        else if(boardValue==nowcolor)
            acstringDeque.push_front('1');
        else
            acstringDeque.push_front('2');
    }
    sign = -sign;
    for (int i = 1; i <8; ++i) {
        int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
        if (!PublicTool::posIsValid(tx, ty)){
            acstringDeque.push_back(2);
            break;
        }
        int boardValue=g.at(tx).at(ty);
        if(boardValue==0)
            acstringDeque.push_back('0');
        else if(boardValue==nowcolor)
            acstringDeque.push_back('1');
        else
            acstringDeque.push_back('2');
    }
    //使用Ac自动机进行匹配
    std::vector<int> tmp = gameModel->acMachine->match(std::string(acstringDeque.begin(),acstringDeque.end()));//返回匹配到的模式串在patterns数组中的编号
    for (int j = 0; j <(int)tmp.size(); j++) {
        ans += patterns[tmp[j]].score;
    }
    return ans;
}

int AlphaBetaAI::getScore(bool color, int x, int y,const std::vector<std::vector<int>>&g){
    int ans = 0;
    int dir = -1;
    int nowcolor = color ? 1 : -1;
    //获取8联通方向的棋子排布
    while (++dir < 4) {
        std::deque<char> acstringDeque;
        acstringDeque.push_back('1');
        int sign = -1;
        for (int i = 1; i < 8; ++i) {
            int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
            if (!PublicTool::posIsValid(tx, ty)){
                acstringDeque.push_front(2);
                break;
            }
            int boardValue=g.at(tx).at(ty);
            if(boardValue==0)
                acstringDeque.push_front('0');
            else if(boardValue==nowcolor)
                acstringDeque.push_front('1');
            else
                acstringDeque.push_front('2');
        }
        sign = -sign;
        for (int i = 1; i <8; ++i) {
            int tx = x + sign * i * dx[dir], ty = y + sign * i * dy[dir];
            if (!PublicTool::posIsValid(tx, ty)){
                acstringDeque.push_back(2);
                break;
            }
            int boardValue=g.at(tx).at(ty);
            if(boardValue==0)
                acstringDeque.push_back('0');
            else if(boardValue==nowcolor)
                acstringDeque.push_back('1');
            else
                acstringDeque.push_back('2');
        }

        //Ac自动机匹配
        std::vector<int> tmp = gameModel->acMachine->match(std::string(acstringDeque.begin(),acstringDeque.end()));
        for (int j = 0; j <(int)tmp.size(); j++) {
            ans += patterns[tmp[j]].score;
        }
    }
    return ans;
}
