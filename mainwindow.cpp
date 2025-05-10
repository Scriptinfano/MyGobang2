#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globals.h"
#include "publictool.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <QMessageBox>
#include <QTimer>
#include "point.h"
#include <QDebug>

MainWindow::MainWindow(std::shared_ptr<DataBaseManager>&theDatabaseManager,const GameType &gametype,const AIType &aitype,const int theUserIndex,QWidget *parent)
    :QDialog(parent)
{
    // 启用鼠标跟踪
    setMouseTracking(true);
    // 设置窗口大小
    int blockTotalLength=BLOCK_SIZE*(BOARD_GRID_SIZE-1);//所有棋盘方格子占据的总长度
    int height=MARGIN * 3 + BLOCK_SIZE * (BOARD_GRID_SIZE-1);
    int width=MARGIN * 2 + blockTotalLength;
    setFixedSize(width,height);

    //赋值数据库接口
    databaseManager=theDatabaseManager;
    userIndex=theUserIndex;

    logger=std::make_unique<TimeLogger>();
    // 初始化游戏
    initGame(gametype,aitype);
    // 初始化算法计时器


}
MainWindow::~MainWindow()
{
}
void MainWindow::initGame(const GameType &gametype,const AIType &aitype)
{
    game = std::make_shared<GameModel>(aitype,gametype);
    localEvaluationAI_ptr=std::make_unique<LocalEvaluationAI>(game);
    alphabetaAI_ptr=std::make_unique<AlphaBetaAI>(game);
    MCTSAI_ptr=std::make_unique<MCTSAI>(game,MCTS_SIMULATION_COUNT,MCTS_TIME_LIMIT);
    game->startGame();
    //算法计时器要知道当前记录的是哪一个算法的时间，这个函数内部会创建一个新文件

    logger->initLogger(game->getAiType());//游戏状态每次重置，也就是每次定出胜负，都要调用此函数，来新建一个日志文件并以新建的日志文件为后续的算法时间输出目的地
    update();
}

static int rowcol2pos(int val)
{
    return MARGIN + val * BLOCK_SIZE;
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(139, 69, 19));       // 设置背景色为棋盘的棕色
    painter.setRenderHint(QPainter::Antialiasing, true); // 启用抗锯齿渲染，平滑图形边缘

    // 设置字体和颜色
    QFont font;
    font.setPointSize(12); // 字体大小你可以自行调整
    painter.setFont(font);
    painter.setPen(Qt::black); // 设置字体颜色

    // 绘制文字内容，你可以自行替换字符串
    QString userText = "当前用户:";
    QString userName=databaseManager->getUserNameByIndex(userIndex);
    userText+=userName;
    painter.drawText(MARGIN, MARGIN / 2, userText);

    // 绘制棋盘线
    for (int i = 0; i < BOARD_GRID_SIZE; i++)
    {
        // 绘制垂直线
        // drawLine的四个参数分别为线段起点和终点的在棋盘上的坐标，坐标系以窗口左上角为原点，水平线为x轴（向右侧增大），垂直线为y轴（向下侧增大）
        painter.drawLine(MARGIN + BLOCK_SIZE * i, MARGIN, MARGIN + BLOCK_SIZE * i, size().height() - 2 * MARGIN);
        // 绘制水平线
        painter.drawLine(MARGIN, MARGIN + BLOCK_SIZE * i, size().width() - MARGIN, MARGIN + BLOCK_SIZE * i);
    }

    // 绘制选中点
    QBrush brush;                     // 画刷用于填充样式、颜色和纹理，绘制填充区域
    brush.setStyle(Qt::SolidPattern); // 设定填充的样式，这里是实心填充样式，即用画刷的当前颜色填充区域
    if (selectPos==true&&PublicTool::posIsValid(clickPosRow,clickPosCol)) {
        //如果某个点被选中了，然后再判断当前玩家的棋子颜色是什么，以绘制对应颜色的提示点
        if (game->getPlayerFlag())
            brush.setColor(Qt::black);
        else
            brush.setColor(Qt::white);
        painter.setBrush(brush);
        //drawRect的四个参数分别为小矩形左上角的横纵坐标和水平方向长度、垂直方向长度
        painter.drawRect(MARGIN + BLOCK_SIZE * clickPosCol - MARK_SIZE / 2,
                         MARGIN + BLOCK_SIZE * clickPosRow - MARK_SIZE / 2,
                         MARK_SIZE,
                         MARK_SIZE);
    }

    //绘制棋子
    for (int i = 0; i < BOARD_GRID_SIZE; i++) {
        for (int j = 0; j < BOARD_GRID_SIZE; j++) {
            //根据当前此处放的是黑棋还是白旗，设定绘制的颜色

            if (game->checkBoardPos(i,j) == BLACK) {
                brush.setColor(Qt::black);
            } else if (game->checkBoardPos(i,j) == WHITE) {
                brush.setColor(Qt::white);
            } else
                continue;
            painter.setBrush(brush);
            painter.drawEllipse(QPoint(rowcol2pos(j),rowcol2pos(i)),
                                CHESS_RADIUS,
                                CHESS_RADIUS); //这是绘制圆形的方法，包括圆和椭圆
        }
    }
}

static bool distanceJudge(int x1, int y1, int x2, int y2)
{
    int len = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
    return len < POS_OFFSET;
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->position().x();
    int y = event->position().y();

    if (x > MARGIN && x < size().width() - MARGIN && y > MARGIN && y < size().height() - 2 * MARGIN)
    {
        // 获取鼠标目前所在棋盘块左上角的点在棋盘中的第几行和第几列（从0开始）
        int row = (y - MARGIN) / BLOCK_SIZE;
        int col = (x - MARGIN) / BLOCK_SIZE;

        // 计算出左上角点的横纵坐标
        int leftTopPosX = MARGIN + BLOCK_SIZE * col;
        int leftTopPosY = MARGIN + BLOCK_SIZE * row;
        // 计算出右上角点的横纵坐标
        int rightTopPosX = leftTopPosX + BLOCK_SIZE;
        int rightTopPosY = leftTopPosY;
        // 计算出左下角点的横纵坐标
        int leftBottomPosX = leftTopPosX;
        int leftBottomPosY = leftTopPosY + BLOCK_SIZE;
        // 计算出右下角点的横纵坐标
        int rightBottomPosX = leftTopPosX + BLOCK_SIZE;
        int rightBottomPosY = leftTopPosY + BLOCK_SIZE;

        // 要在鼠标当前位置所在的方块的四个角的哪一个点上绘制提示点，要有两个条件满足，一是不能有棋子，二是鼠标当前位置和该点的距离要小于全局变量POS_OFFSET
        if (distanceJudge(x, y, leftTopPosX, leftTopPosY) && game->checkBoardPos(row,col) == EMPTY)
        {
            clickPosRow = row;
            clickPosCol = col;
            selectPos = true;
            update();
            return;
        }
        if (distanceJudge(x, y, rightTopPosX, rightTopPosY) && game->checkBoardPos(row,col+1) == EMPTY)
        {
            clickPosRow = row;
            clickPosCol = col + 1;
            selectPos = true;
            update();
            return;
        }
        if (distanceJudge(x, y, leftBottomPosX, leftBottomPosY) && game->checkBoardPos(row+1,col) == EMPTY)
        {
            clickPosRow = row + 1;
            clickPosCol = col;
            selectPos = true;
            update();
            return;
        }
        if (distanceJudge(x, y, rightBottomPosX, rightBottomPosY) && game->checkBoardPos(row+1,col+1) == EMPTY)
        {
            clickPosRow = row + 1;
            clickPosCol = col + 1;
            selectPos = true;
            update();
            return;
        }
    }
    selectPos=false;
}

bool MainWindow::judgeWinOrLose(){
    // 判断输赢
    // 只要棋步里现在有棋子，就可以开始判断
    if (game->steps.size()) {
        Point p=game->steps.back();
        int x=p.x,y=p.y;
        if (game->isWin(x, y)) {
            QString str;
            if (p.color == true){
                str = "黑棋";
                databaseManager->recordWin(userIndex);
            }
            else if (p.color == false){
                str = "白棋";
                databaseManager->recordLose(userIndex);
            }
            game->setGameStatus(WIN);
            QMessageBox::StandardButton btnValue = QMessageBox::information(this,"游戏结束",str + "胜利");
            if (btnValue == QMessageBox::Ok) {
                // 重置游戏状态，重新开始游戏
                game->startGame();
                logger->initLogger(game->getAiType());
                return true;
            }
        }else if(game->isDeadGame()){
            QMessageBox::StandardButton btnValue = QMessageBox::information(this,"游戏结束","平局");
            if (btnValue == QMessageBox::Ok) {
                // 重置游戏状态，重新开始游戏
                game->startGame();
                logger->initLogger(game->getAiType());
                return true;
            }
        }
    }
    return false;
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    //判断此时有没有点被选中，如果选中的话才能继续
    if (selectPos == false)
        return;
    else
        selectPos = false;
    chessOneByPerson();
    bool isEnd=judgeWinOrLose();//人落子之后判断一下输赢
    if(!isEnd&&game->getGameType()==AI){
        chessOneByAI();
        judgeWinOrLose();//AI落子之后再判断一下输赢
    }
}

void MainWindow::chessOneByPerson() {
    if(clickPosRow!=-1&&clickPosCol!=-1&&game->checkBoardPos(clickPosRow,clickPosCol)==EMPTY){
        game->updateGameModel(clickPosRow,clickPosCol);
        update();//update函数会激活paintEvent事件，会重新绘制所有元素，包括棋子
    }
}

void MainWindow::chessOneByAI() {
    switch (game->getAiType()) {
        case LOCALEVALUATION: {
            logger->startTiming();
            Point p=localEvaluationAI_ptr->getNextStep();
            logger->endTimingAndLog();
            clickPosRow = p.x;
            clickPosCol = p.y;
            break;
        }
        case MCTS: {
            logger->startTiming();
            std::pair<int,int>p=MCTSAI_ptr->getBestMove();
            logger->endTimingAndLog();
            clickPosRow=p.first;
            clickPosCol=p.second;
            break;
        }
        case ALPHABETA: {
            logger->startTiming();//算法运行时间的记录
            Point p=alphabetaAI_ptr->getBestMove();
            logger->endTimingAndLog();
            clickPosRow = p.x;
            clickPosCol = p.y;
            break;
        }
    }
    //TODO 以下几行代码仅测试使用，输出每次AI做出决策之后，val的最大和最小分别是多少，默认最小是0
    qDebug()<<"MAXVAL="<<MAXVAL<<",MINVAL="<<MINVAL;
    game->updateGameModel(clickPosRow,clickPosCol);
    update();
}


