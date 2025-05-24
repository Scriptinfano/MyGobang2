#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

#include "gamemodel.h"
#include "localevaluationai.h"
#include "alphabetaai.h"
#include "mctsai.h"
#include "databasemanager.h"
#include "timelogger.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<DataBaseManager> &theDatabaseManager, const GameType &gametype, const AIType &aitype, const int theUserIndex, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    /**
     * @brief closeEvent 窗口关闭时触发此事件
     * @param event
     */
    void closeEvent(QCloseEvent *event) override;

private:
    // 数据库控制接口
    std::shared_ptr<DataBaseManager> databaseManager;

    // 数据层访问接口，底下三个ＡＩ也要取得对于数据层的访问权限，所以这里设置为shared_ptr
    std::shared_ptr<GameModel> game;

    // 下面是三个AI对象，负责算法的运算，也就是运算层的访问接口

    std::unique_ptr<LocalEvaluationAI> localEvaluationAI_ptr;
    std::unique_ptr<AlphaBetaAI> alphabetaAI_ptr;
    std::unique_ptr<MCTSAI> MCTSAI_ptr;

    int clickPosRow = -1;   // 即将绘制在棋盘上的棋子的横坐标
    int clickPosCol = -1;   // 即将绘制在棋盘上的棋子的横坐标
    bool selectPos = false; // 是否有位置被选中了
    int userIndex = 1;      // 当前用户是谁

    // 算法测时器
    std::unique_ptr<TimeLogger> logger;

    /**
     * @brief initGame 在构造函数中调用，开始初始化游戏的数据
     * @param gametype 游戏的类型
     * @param aitype AI的类型
     */
    void initGame(const GameType &gametype, const AIType &aitype);

    /**
     * @brief paintEvent 事件处理函数，用于处理窗口或部件需要重绘时的事件；当QT的窗口部件需要绘制或更新时会触发这个函数，开发者应重写此函数来定义自己的窗口或控件绘制行为
     * @param event 事件对象的指针
     */
    void paintEvent(QPaintEvent *event) override;
    /**
     * @brief mouseMoveEvent 鼠标移动时触发此事件
     * @param event
     */
    void mouseMoveEvent(QMouseEvent *event) override;
    /**
     * @brief mouseReleaseEvent 鼠标松开时触发此事件
     */
    void mouseReleaseEvent(QMouseEvent *event) override;
    /**
     * @brief chessOneByPerson 人类落子
     */
    void chessOneByPerson();
    /**
     * @brief chessOneByAI AI落子
     */
    void chessOneByAI();
    /**
     * @brief judgeWinOrLose 判断输赢，如果分出胜负了，或者平局了，只要游戏结束都会返回true，如果游戏没有结束，那么仍然返回false
     * @return
     */
    bool judgeWinOrLose();
    /**
     * 主要用来清理多余的日志文件，因为如果重新初始化游戏之后，会创建一个新的空日志文件，如果此时点击关闭，那么空日志文件就会堆积，需要清理
     */
    void deleteLogFiles();
};
#endif // MAINWINDOW_H
