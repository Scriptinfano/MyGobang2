#include "zobristhash.h"
#include "globals.h"
#include <random>
#include <nlohmann/json.hpp>
#include <QStandardPaths>
#include <QDir>
#include <fstream>
#include <QMessageBox>
#include <QCoreApplication>

ZobristHash::ZobristHash()
{
    for (int i = 0; i < BOARD_GRID_SIZE; i++)
        for (int j = 0; j < BOARD_GRID_SIZE; j++)
        {
            zobristBlack[i][j] = getRandom64();
            zobristWhite[i][j] = getRandom64();
        }
}

bool ZobristHash::hasKey(ll key)
{
    return zobristList.count(key);
}
bool ZobristHash::getHashValue(ll key, bool color, int &res)
{
    if (zobristList.count(key))
    {
        res = color ? zobristList[key] : -zobristList[key];
        return true;
    }
    return false;
}
ll ZobristHash::getZobristHash(std::vector<Point> state)
{
    ll key = 0;
    for (auto s : state)
    {
        int x = s.x, y = s.y, color = s.color;
        if (color)
        {
            key ^= zobristBlack[x][y];
        }
        else
        {
            key ^= zobristWhite[x][y];
        }
    }
    return key;
}
ll ZobristHash::getRandom64()
{
    static std::mt19937_64 rng(std::random_device{}()); // 64 位 Mersenne Twister 随机数生成器
    return rng();
}
void ZobristHash::updateHashValue(ll key, int value)
{
    zobristList[key] = value;
}

void ZobristHash::saveZobristHash()
{
    try
    {
        nlohmann::json j; // nlohmann::json对象在插入键值对时，如果键已存在，新值会覆盖旧值。
        for (const auto &kv : zobristList)
        {
            j[std::to_string(kv.first)] = kv.second;
        }
        // 代码中遍历zobristList时，如果zobristList中有重复的键，那么最后写入JSON的将是最后一次遍历到的值
        std::ofstream file(getDataFilePath());
        file << j.dump(4); // 把nlohmann::json对象j格式化为带有4个空格缩进的JSON字符串，并写入到file（即zobristlist.json文件）中
        
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(nullptr, "错误", "无法保存Zobrist哈希表，程序即将退出");
        QCoreApplication::exit(-1); // 或 QApplication::exit(-1);
    }
}

void ZobristHash::loadZobristHash()
{
    try
    {
        std::ifstream file(getDataFilePath());
        if (!file.is_open())
        {
            // 文件不存在，直接退出
            QMessageBox::critical(nullptr, "错误", "无法找到存储Zobrist哈希表的文件，程序即将退出。");
            QCoreApplication::exit(-1); // 或 QApplication::exit(-1);
        }
        nlohmann::json j;
        file >> j;
        zobristList.clear();
        for (auto it = j.begin(); it != j.end(); ++it)
        {
            ll key = std::stoll(it.key()); // std::stoll 是 C++ 标准库中的一个函数，全称是 "string to long long"
            int value = it.value();
            zobristList[key] = value;
        }
    }
    catch (const std::exception &e)
    {
        // 读取失败，zobristList保持为空
        zobristList.clear();
        QMessageBox::critical(nullptr, "错误", "无法加载Zobrist哈希表，程序即将退出");
        QCoreApplication::exit(-1); // 或 QApplication::exit(-1);
    }
}

std::string ZobristHash::getDataFilePath()
{
    // 使用Qt的QStandardPaths获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // 确保目录存在
    QDir dir(dataPath);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    // 构建完整的文件路径
    QString filePath = dataPath + "/zobristlist.json";

    // 转换为标准字符串并返回
    return filePath.toStdString();
}
