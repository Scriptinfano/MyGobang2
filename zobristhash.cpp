#include "zobristhash.h"
#include "globals.h"
#include <random>
ZobristHash::ZobristHash()
{
    for(int i = 0;i<BOARD_GRID_SIZE;i++)
        for(int j =0 ;j<BOARD_GRID_SIZE;j++){
            zobristBlack[i][j] = getRandom64();
            zobristWhite[i][j] = getRandom64();
        }
}

bool ZobristHash::hasKey(ll key){
    return zobristList.count(key);
}
bool ZobristHash::getHashValue(ll key,bool color,int &res){
    if(zobristList.count(key)){
        res= color ? zobristList[key]:-zobristList[key];
        return true;
    }
    return false;
}
ll ZobristHash::getZobristHash(std::vector<Point>state){
    ll key=0;
    for(auto s:state){
        int x=s.x,y=s.y,color=s.color;
        if(color){
            key^=zobristBlack[x][y];
        }else{
            key^=zobristWhite[x][y];
        }
    }
    return key;
}
ll ZobristHash::getRandom64(){
    static std::mt19937_64 rng(std::random_device{}());  // 64 位 Mersenne Twister 随机数生成器
    return rng();
}
void ZobristHash::updateHashValue(ll key,int value){
    zobristList[key]=value;
}
