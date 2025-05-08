#include "mctsai.h"
#include "globals.h"
#include "publictool.h"

#include <unordered_set>
#include <stdexcept>
#include "pairhash.h"
#include <map>
#include <cmath>
#include "pattern.h"
void MCTSNode::traverse(std::set<std::pair<int,int>>&s,Point&p,int i,int sign){

    int tx = p.x+sign*dx[i];
    int ty = p.y+sign*dy[i];
    if (!PublicTool::posIsValid(tx,ty)||mctsai_ptr->gameModel_ptr->getBoardValue(tx,ty)!=0){
        return;
    }
    if (!s.contains({tx,ty})){
        s.insert({tx,ty});
    }
}
std::vector<Point> MCTSNode::getPossibleMoves()
{
    std::vector<Point> possibleMoves;
    std::set<std::pair<int,int>>s;
    for (auto p:state)
    {
        for(int i=0;i<4;i++){
            int sign=1;
            traverse(s,p,i,sign);
            sign=-sign;
            traverse(s,p,i,sign);
        }
    }
    for(auto p:s){
        possibleMoves.push_back(Point(nextColor,p.first,p.second));
    }
    return std::move(possibleMoves);
}

std::stack<Point> MCTSNode::getPossibleMovesWithEvaluation()
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
    for (int i=0;i<state.size();i++){
        Point p=state[i];
        pairSet.insert({ p.x,p.y });
    }

    for (int i=0;i<state.size();i++) {
        //扫描每个已落子点周围的八个点
        Point point=state[i];
        for (int i = 0; i < 8; i++) {
            int tx = point.x + dtx[i], ty = point.y + dty[i];
            //扫描到一个非法位置，或者扫描到一个已经落子的位置则继续绕圈
            if (!PublicTool::posIsValid(tx,ty))
                continue;
            //之前这个合法空点已经被扫描过了
            if (pairSet.count({ tx, ty }))
                continue;
            //将未被扫描的合法点加入已扫描集合
            pairSet.insert({ tx, ty });
            //模拟在扫描到的点上落子
            state.push_back(Point(nextColor,tx,ty));
            //让下一步的当前子的val尽可能的大
            //然后评估模拟落子之后的局面
            scoreMap.insert({ mctsai_ptr->evaluateState(state, nextColor), {tx ,ty} });
            state.pop_back();
        }
    }
    //将最后结果加入stack中（从小到大）
    std::stack<Point> ans;
    for (const auto& [key, value] : scoreMap) {
        int x = value.first, y = value.second;
        ans.push(Point(nextColor,x,y,key));
    }
    return ans;
}

void MCTSNode::expandNodes(){
    //这里的getPossibleMovesWithEvaluation用启发式的，每个节点的靠前的孩子节点会更优一点
    std::stack<Point>possibleMoves=getPossibleMovesWithEvaluation();
    if(children.size()==0){
        while(possibleMoves.size()!=0){
            Point p=possibleMoves.top();
            possibleMoves.pop();
            std::vector<Point>newState=state;//发生拷贝构造，克隆一个新的容器，和原来一样
            newState.push_back(Point(nextColor,p.x,p.y));
            children.push_back(std::make_shared<MCTSNode>(newState,shared_from_this(),!nextColor,mctsai_ptr));
        }
    }
}

/**
 * @brief UCB 增强版的UCT公式，这个函数不要放到任何头文件中，该函数仅为MCTSNode::calculateValue服务
 * @param w w是权重因子，范围[0,1]，控制评估分数对UCT的影响程度
 * @param winRate 传统UCB公式中的胜率，由于val无论对黑子还是白子来说都是越大越好，所以这里的胜率一定要根据黑子还是白子做相应的转换
 * @param val αβ评估函数给出的分数，已经考虑过黑白子的影响了
 * @param lambda λ 是缩放系数，控制评估分数对归一化的影响强度（通常取 0.001 左右）
 * @param C 传统UCB公式中的探索系数，取根号2或者2，在深入探索和另辟蹊径之间的平衡
 * @param totalVisits 父节点的访问次数
 * @param selfVisits 当前节点的访问次数
 * @return 返回增强UCB值
 */
double UCB(double w, double winRate, double val,double lambda, double C, double totalVisits,double selfVisits) {
    double eval_norm_value = 1 / (1 + std::exp(-lambda * val));//对val做归一化处理
    return (1 - w) * winRate + w * eval_norm_value + C * std::sqrt(std::log(totalVisits) / selfVisits);
}
double MCTSNode::calculateValue(){

    if(parent==nullptr){
        throw std::runtime_error("calculate value for node with no parent");
    }
    /*
    ucb = math.inf
    if self.parent.visits == 0 or self.visits == 0:
        return ucb
    rate = self.wins/self.visits # 算的是从父状态到当前状态所落子后黑子的胜率
    # 这里还要区分一下下一步落子的颜色，计算胜率的时候会有所不同，因为wins代表的是黑色节点胜利的次数，对于白色节点来说要用1-黑色节点胜率
    # 如果当前状态下该落白子了，说明导致该局面的是一枚黑子的落下，所以当self.nowColor为-1时，胜率应该是rate，否则应该是1-rate
    winRate = rate if self.nowColor == -1 else 1-rate
    ucb = winRate + UCB_WEIGHTS * math.sqrt(math.log(self.parent.visits) / self.visits)
    return ucb
    */
    double ucb=DOUBLE_INF;
    //当前节点的探索次数是0会导致ucb val无穷大
    if(visits==0){
        enhenced_ucb_val=ucb;
        return ucb;
    }
    double rate=wins/visits;
    double winRate=nextColor==false?rate:1-rate;
    //这个ucb公式将结合评估分数和传统的ucb公式
    int val=mctsai_ptr->evaluateState(state,nextColor);
    enhenced_ucb_val=UCB(EVAL_UCB_WEIGHT,winRate,val,NORMALIZE_LAMBDA,EXPLORATION_WEIGHT,parent->visits,visits);
    return enhenced_ucb_val;
}
std::shared_ptr<MCTSNode> MCTSNode::getBestChild()
{
    double best_enhenced_ucb_val=-DOUBLE_INF;
    std::vector<std::shared_ptr<MCTSNode>> bestChildren;//因为只返回靠前的child，所以即便后面有分数一样优秀的节点一概也不考虑，所以不会记录分数同等优秀的节点，因为前面用了启发式添加子节点
    if(children.size()!=0){

        for(int i=0;i<children.size();i++){
            children[i]->calculateValue();
        }

        for(int i=0;i<children.size();i++){
            if(children[i]->enhenced_ucb_val>best_enhenced_ucb_val)
            {
                bestChildren.clear();
                best_enhenced_ucb_val=children[i]->enhenced_ucb_val;
                bestChildren.push_back(children[i]);
            }else if(children[i]->enhenced_ucb_val==best_enhenced_ucb_val){
                bestChildren.push_back(children[i]);
            }
        }

        std::random_device rd;  // 获取随机种子
        std::mt19937 gen(rd());  // 生成器
        std::uniform_int_distribution<> dis(0, bestChildren.size()-1);  // 均匀分布
        // 生成一个范围内的随机整数
        int randomIndex = dis(gen);
        if(bestChildren.size()==0)
            return children[0];
        return bestChildren[randomIndex];
    }else{
        return nullptr;
    }
}

std::shared_ptr<MCTSNode> MCTSNode::getFinalBestChild(){
    double best_enhenced_ucb_val=-DOUBLE_INF;
    std::shared_ptr<MCTSNode> bestChild;
    if(children.size()!=0){
        //TODO 这里有没有必要再计算一遍val
        for(int i=0;i<children.size();i++){
            children[i]->calculateValue();
        }
        bestChild=children[0];//默认返回第一个子节点，除非后面的计算出来的val比前面的大
        for(int i=0;i<children.size();i++){
            if(children[i]->enhenced_ucb_val>best_enhenced_ucb_val)
            {
                best_enhenced_ucb_val=children[i]->enhenced_ucb_val;
                bestChild=children[i];
            }
        }
        return bestChild;
    }else{
        return nullptr;
    }
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
MCTSAI::MCTSAI(const std::shared_ptr<GameModel>& theGameModel ,int simulations, double timeLimitSec):
    rng(std::random_device{}()),
    simulationCount(simulations),
    timeLimit(timeLimitSec),
    gameModel_ptr(theGameModel)
{
    zobristHash_ptr=std::make_unique<ZobristHash>();
    root=nullptr;
}

int MCTSAI::evaluateState(std::vector<Point> &state, bool nextcolor)
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
                int boardValue=gameModel_ptr->getBoardValue(tx,ty);
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
                int boardValue=gameModel_ptr->getBoardValue(tx,ty);
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
    return val;
}

int MCTSAI::getScoreInDir(bool color,int x,int y,std::vector<Point> state,int dir){
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
    std::vector<int> tmp = gameModel_ptr->acMachine->match(std::string(acstringDeque.begin(),acstringDeque.end()));//返回匹配到的模式串在patterns数组中的编号
    for (int j = 0; j <(int)tmp.size(); j++) {
        ans += patterns[tmp[j]].score;
    }
    return ans;
}

int MCTSAI::getScore(bool color, int x, int y,const std::vector<std::vector<int>>&g){
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
        std::vector<int> tmp = gameModel_ptr->acMachine->match(std::string(acstringDeque.begin(),acstringDeque.end()));
        for (int j = 0; j <(int)tmp.size(); j++) {
            ans += patterns[tmp[j]].score;
        }
    }
    return ans;
}

std::pair<int, int> MCTSAI::getBestMove()
{
    std::pair<Point, std::shared_ptr<MCTSNode>>res=mctsIterationLoop();
    res.second->nextColor=gameModel_ptr->steps.back().color;
    res.second->parent=nullptr;
    res.second->state=gameModel_ptr->steps;
    root=res.second;
    return {res.first.x,res.first.y};
}


std::pair<Point, std::shared_ptr<MCTSNode>> MCTSAI::mctsIterationLoop()
{

    root=std::make_shared<MCTSNode>(gameModel_ptr->steps,nullptr,false,shared_from_this());
    std::shared_ptr<MCTSNode>node=root;
    int iterNum=0;
    while(iterNum<MCTS_SIMULATION_COUNT){
        std::pair<std::shared_ptr<MCTSNode>, bool>selectionRes=mctsSelection(node);
        if(selectionRes.second&&selectionRes.first->visits==0){
            std::shared_ptr<MCTSNode>endNode=mctsSimulation(selectionRes.first);
            mctsBackPropagation(endNode,selectionRes.first);
        }else{
            mctsExpansion(selectionRes.first);
        }
        iterNum++;
        node=root;
    }
    std::shared_ptr<MCTSNode>bestChild=root->getFinalBestChild();
    if(bestChild==nullptr)
        throw std::runtime_error("无法获取bestChild，因为根节点没有子节点，检查代码逻辑");
    return {bestChild->state.back(),bestChild};
}

std::pair<std::shared_ptr<MCTSNode>, bool> MCTSAI::mctsSelection(std::shared_ptr<MCTSNode>node)
{
    bool hasIter=false;
    while(node->children.size()!=0){
        node=node->getBestChild();
        hasIter=true;
    }
    return {node,hasIter};
}

void MCTSAI::mctsExpansion(std::shared_ptr<MCTSNode>node)
{
    node->expandNodes();
}

std::shared_ptr<MCTSNode> MCTSAI::mctsSimulation(std::shared_ptr<MCTSNode>node)
{
    while(!PublicTool::checkWin(node->state)){
        std::stack<Point>possibleMoves=node->getPossibleMovesWithEvaluation();
        std::vector<Point>newState=node->state;//触发拷贝构造
        newState.push_back(possibleMoves.top());//取出最顶部的最优秀的节点
        node=std::make_shared<MCTSNode>(newState,node,!node->nextColor,node->mctsai_ptr);
    }
    return node;
}

void MCTSAI::mctsBackPropagation(std::shared_ptr<MCTSNode>endNode,std::shared_ptr<MCTSNode>node)
{
    /*
    将endNode的价值，也就是胜率计算出来，然后沿着node一路向上反向传播更新节点的关键值
    先判断一下最后到底是输了还是赢了，所有相关信息都存储到了endNode中，只要看一下endNode保存的棋步数组的最后一个点是什么颜色就行
    */
    bool resColor=endNode->state.back().color;
    while(node!=nullptr){
        node->wins += resColor==1?1:0;
        node->visits+=1;
        node=node->parent;
    }
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
