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
    elapesedTimesVec.push_back(elapsedMs);
    if (file.isOpen())
    {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " - Step " << stepCounter << ": " << elapsedMs << " ms\n";
    }
    qDebug() << "Step" << stepCounter << "耗时：" << elapsedMs << "ms";
}

void TimeLogger::calculateAnalyzeInfo()
{
    if (file.isOpen())
    {
        qint64 total = 0;
        qint64 maximumValue = 0;
        for (auto &item : elapesedTimesVec)
        {
            // 1、算一下所有时间的平均响应时间
            total += item;
            // 2、算一下最大响应时间
            if (item > maximumValue)
                maximumValue = item;
        }

        qint64 count = elapesedTimesVec.size();
        qint64 res = count > 0 ? total / count : 0;

        // 3、计算标准差（使用样本标准差公式）
        double varianceSum = 0.0;
        for (auto &item : elapesedTimesVec)
        {
            double diff = static_cast<double>(item) - static_cast<double>(res);
            varianceSum += diff * diff;
        }

        double stddev = 0.0;
        if (count > 1)
            stddev = std::sqrt(varianceSum / (count - 1));

        QTextStream out(&file);
        out << "平均响应时长=" << res
            << ";最大响应时间=" << maximumValue
            << ";总耗时=" << total
            << ";标准差=" << stddev << "\n";

        // 在终端也输出统计信息
        qDebug() << "平均响应时长=" << res
                 << ";最大响应时间=" << maximumValue
                 << ";总耗时=" << total
                 << ";标准差=" << stddev;
    }
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
static QString gameTypeToString(GameType type)
{
    switch (type)
    {
    case GameType::AI:
        return "AI";
    case GameType::AIAI:
        return "AIAI";
    case GameType::MAN:
        return "MAN";
    default:
        return "UNKNOWN";
    }
}
void TimeLogger::initLogger(GameType gameType, AIType aiType, AIType otherAiType)
{

    // 日志模块统计所记录的信息做相关计算
    calculateAnalyzeInfo();

    // 重新绑定新的文件
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

    //  Step 2: 构造文件名
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString aiTypeStr = aiTypeToString(aiType);
    QString otherAiTypeStr = aiTypeToString(otherAiType);
    QString gameTypeStr = gameTypeToString(gameType);
    QString fileName;
    if (gameType != AIAI)
    {
        fileName = QString("%1/%2_%3_%4.txt").arg(logDirPath, timestamp, gameTypeStr, aiTypeStr);
    }
    else
    {
        fileName = QString("%1/%2_%3_%4_%5.txt").arg(logDirPath, timestamp, gameTypeStr, aiTypeStr, otherAiTypeStr);
    }

    // Step 3: 绑定新的文件名字，旧的已绑定文件，QT会自动关闭
    file.setFileName(fileName);

    // Step 4: 打开文件
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "无法打开日志文件：" << fileName;
        qDebug() << "错误信息：" << file.errorString();
    }
    // 重置相关成员变量
    stepCounter = 0;
    elapesedTimesVec.clear();
}

void TimeLogger::recordWinOrLose(char tag)
{

    if (file.isOpen())
    {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " - Win or Lose: " << tag << "\n";
    }
}
