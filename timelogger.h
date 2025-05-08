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

    void endTimingAndLog(int stepNumber);

    /**
     * @brief initLogger 重新创建一个新文件，并以新文件作为新的日志输出目的地，并将当前记录算法的名字记录在文件的名字上
     * @param type
     */
    void initLogger(AIType type);
private:
    QElapsedTimer timer;

    std::unique_ptr<QFile> file;

    int stepCounter=0;//每一局游戏中的步数，initLogger的时候请归零
};

#endif // TIMELOGGER_H
