#include "timelogger.h"
#include <QDir>      // QDir 类，用于处理目录操作
#include <QDateTime> // QDateTime 类，用于获取时间戳
#include <QString>   // QString 字符串操作
#include <QDebug>    // 用于调试输出日志
#include <QtGlobal>
#include <QMessageBox>      // 添加 QMessageBox 头文件
#include <QCoreApplication> // 添加 QCoreApplication 头文件
TimeLogger::TimeLogger()
{
}

void TimeLogger::startTiming()
{
    timer.start();
}

void TimeLogger::endTimingAndLog()
{
    stepCounter++;
    qint64 elapsedMs = timer.elapsed();
    if (file.isOpen())
    {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " - Step " << stepCounter << ": " << elapsedMs << " ms\n";
    }
    qDebug() << "Step" << stepCounter << "耗时：" << elapsedMs << "ms";
    
}

QString aiTypeToString(AIType type)
{
    switch (type)
    {
    case AIType::LOCALEVALUATION:
        return "LOCALEVALUATION";
    case AIType::MCTS:
        return "MCTS";
    case AIType::ALPHABETA:
        return "ALPHABETA";
    default:
        return "UNKNOWN";
    }
}
void TimeLogger::initLogger(AIType type)
{
    qDebug() << "Current working dir: " << QDir::currentPath();
    // Step 1: 确保 logs 目录存在
    QString logDirPath = "/Users/mavrick/QTProjects/MyGoBang2/logs";
    QDir logDir(logDirPath);
    if (!logDir.exists())
    {
        QMessageBox::warning(nullptr, "日志目录错误",
                             "请设置日志目录或者删除日志记录模块",
                             QMessageBox::Ok);
        QCoreApplication::exit(1); // 直接结束程序，返回错误码1
    }
    // stepCounter=0;
    //  Step 2: 构造文件名
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString typeStr = aiTypeToString(type);
    QString fileName = QString("%1/%2_%3.txt").arg(logDirPath, timestamp, typeStr);

    // Step 3: 绑定文件名字

    file.setFileName(fileName);

    // Step 4: 打开文件
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开日志文件：" << fileName;
        qDebug() << "错误信息：" << file.errorString();
    }
    stepCounter = 0;
}
