#include "databasemanager.h"

#include <QDebug>
#include <QCoreApplication>
DataBaseManager::DataBaseManager(QObject *parent)
    : QObject{parent}
{
    openDatabase();
}
DataBaseManager::~DataBaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DataBaseManager::openDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    QString path = QCoreApplication::applicationDirPath() + "/../Resources/data/gomoku.db";
    db.setDatabaseName(path);
    qDebug()<<QCoreApplication::applicationDirPath();
    if (!db.open()) {
        qWarning() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    qDebug() << "Available drivers:" << QSqlDatabase::drivers();
    qDebug() << "Database opened at:" << path;
    return true;
}

bool DataBaseManager::addUser(const QString& username) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username) VALUES (:username)");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qWarning() << "Add user failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManager::recordWin(int index) {
    QSqlQuery query;
    // 查询当前胜场与总场
    query.prepare("SELECT wins, total FROM users WHERE id = :id");
    query.bindValue(":id", index);

    if (!query.exec() || !query.next()) {
        qWarning() << "User not found or query failed (win):" << query.lastError().text();
        return false;
    }

    int wins = query.value("wins").toInt();
    int total = query.value("total").toInt();
    wins += 1;
    total += 1;
    double winrate = total > 0 ? static_cast<double>(wins) / total : 0.0;

    // 更新数据库
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE users SET wins = :wins, total = :total, winrate = :winrate WHERE id = :id");
    updateQuery.bindValue(":wins", wins);
    updateQuery.bindValue(":total", total);
    updateQuery.bindValue(":winrate", winrate);
    updateQuery.bindValue(":id", index);

    if (!updateQuery.exec()) {
        qWarning() << "Update failed (win):" << updateQuery.lastError().text();
        return false;
    }

    return true;
}

bool DataBaseManager::recordLose(int index) {
    QSqlQuery query;
    // 查询当前胜场与总场
    query.prepare("SELECT wins, total FROM users WHERE id = :id");
    query.bindValue(":id", index);

    if (!query.exec() || !query.next()) {
        qWarning() << "User not found or query failed (lose):" << query.lastError().text();
        return false;
    }

    int wins = query.value("wins").toInt();
    int total = query.value("total").toInt();
    total += 1;
    double winrate = total > 0 ? static_cast<double>(wins) / total : 0.0;

    // 更新数据库
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE users SET total = :total, winrate = :winrate WHERE id = :id");
    updateQuery.bindValue(":total", total);
    updateQuery.bindValue(":winrate", winrate);
    updateQuery.bindValue(":id", index);

    if (!updateQuery.exec()) {
        qWarning() << "Update failed (lose):" << updateQuery.lastError().text();
        return false;
    }

    return true;
}


QStringList DataBaseManager::getAllUsernames() {
    QStringList usernames;
    QSqlQuery query("SELECT username FROM users");

    if (!query.exec()) {
        qWarning() << "Failed to fetch usernames:" << query.lastError().text();
        return usernames;
    }

    while (query.next()) {
        QString name = query.value(0).toString();
        usernames << name;
    }

    return usernames;
}

QString DataBaseManager::getUserNameByIndex(int index) {
    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE id = :index");
    query.bindValue(":index", index);

    if (!query.exec()) {
        qWarning() << "Query failed:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString(); // 获取 username 字段
    } else {
        qWarning() << "No user found with id:" << index;
        return QString();
    }
}
