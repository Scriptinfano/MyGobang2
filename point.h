#ifndef POINT_H
#define POINT_H
/**
 * @brief The Point class 棋步，每下一步棋，就会将这步棋相关的信息存储到这个对象中，最后将所有棋步全部放到一个vector中，代表一盘棋中所有棋子落子的先后顺序
 */
class Point{
public:
     bool color;
     int x,y,score;

     //两个构造函数，区别在于能不能给score赋值
     Point(bool color, int x, int y)
     {
         this->color = color;
         this->x = x;
         this->y = y;
         this->score = 0;
     }
     Point(bool color, int x, int y,int score)
     {
         this->color = color;
         this->x = x;
         this->y = y;
         this->score = score;
     }
};
#endif // POINT_H
