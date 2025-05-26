#ifndef GLOBALS_H
#define GLOBALS_H
constexpr int BOARD_GRID_SIZE = 16; //棋子在棋盘上最多可以落几个子，用这个变量去初始化代表棋盘数据的二维数组
constexpr int MARGIN = 30; //棋盘边缘距离窗口边缘有多远
constexpr int CHESS_RADIUS = 15; //棋子的半径
constexpr int MARK_SIZE = 6; //落子标记边长，还没落子的时候标记的小正方形有多大
constexpr int BLOCK_SIZE = 40; //正方形格子的边长
constexpr int POS_OFFSET = BLOCK_SIZE  * 0.4; //鼠标点击的模糊距离上限，鼠标在接近棋盘线交叉点多远的时候显示标记点
constexpr int AI_THINK_TIME=30;//仅适用于最简单的AI算法，模拟AI思考的时间

//以下三个常量代表不同难度等级下针对αβ剪枝搜索最大搜索深度
constexpr int GLOBALDEPTHSIX = 6;
constexpr int GLOBALDEPTHFOUR = 4;
constexpr int GLOBALDEPTHEIGHT = 8;

extern const int INF;		   // 定义整数无穷大
extern const double DOUBLE_INF;//定义小数无穷大
constexpr int MAXDEPTH=6;//最大搜索深度
extern const int dx[4];
extern const int dy[4];
//下面两个变量是用来测试评估函数返回的最大最小值是多少的
extern int MINVAL;
extern int MAXVAL;
#endif // GLOBALS_H

