#include "globals.h"
#include<limits>
const int dx[] = { 1, 0, 1, 1 }, dy[] = { 0, 1, 1, -1 };//四个联通方向的偏移量数组
const int INF=std::numeric_limits<int>::max();
const double DOUBLE_INF=std::numeric_limits<double>::max();
int MINVAL=0;//预估最小值-3020
int MAXVAL=0;//预估最大值3940
