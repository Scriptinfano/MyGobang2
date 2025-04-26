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
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username) VALUES (:username)");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qWarning() << "Add user failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManager::recordWin(int index) {
    QSqlQuery query(db);
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
    QSqlQuery updateQuery(db);
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
    QSqlQuery query(db);
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
    QSqlQuery updateQuery(db);
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
    QSqlQuery query(db);
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
QList<UserRecord> DataBaseManager::getAllUsers() {
    QList<UserRecord> users;
    QSqlQuery query("SELECT id, username, wins, total, winrate FROM users");

    while (query.next()) {
        UserRecord record;
        record.id = query.value("id").toInt();
        record.username = query.value("username").toString();
        record.wins = query.value("wins").toInt();
        record.total = query.value("total").toInt();
        record.winrate = query.value("winrate").toDouble();
        users.append(record);
    }

    return users;
}

bool DataBaseManager::deleteUser(int id) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qWarning() << "Failed to delete user:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManager::updateUsername(int id, const QString& newUsername) {
    QSqlQuery query(db);
    query.prepare("UPDATE users SET username = :username WHERE id = :id");
    query.bindValue(":username", newUsername);
    query.bindValue(":id", id);
    if (!query.exec()) {
        qWarning() << "Failed to update username:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DataBaseManager::isUsernameExists(const QString& username) {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec() || !query.next()) {
        qWarning() << "Username check failed:" << query.lastError().text();
        return false;
    }
    return query.value(0).toInt() > 0;
}

