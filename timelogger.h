#ifndef TIMELOGGER_H
#define TIMELOGGER_H


#include <QElapsedTimer>
#include <QFile>
#include "gamemodel.h"
class TimeLogger
{
public:
    TimeLogger();

    void startTiming();

    void endTimingAndLog();

    /**
     * @brief initLogger 重新创建一个新文件，并以新文件作为新的日志输出目的地，并将当前记录算法的名字记录在文件的名字上
     * @param type
     */
    void initLogger(AIType type);

    /**
     * @brief recordWinOrLose 记录每局游戏的结果，tag为W表示胜利，tag为L表示失败，tag为D表示平局
     * @param tag
     */
    void recordWinOrLose(char tag);
private:
    QElapsedTimer timer;

    QFile file;

    int stepCounter=0;//每一局游戏中的步数，initLogger的时候请归零
};

#endif // TIMELOGGER_H
