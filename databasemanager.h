#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>

struct UserRecord {
    int id;
    QString username;
    int wins;
    int total;
    double winrate;
};

class DataBaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseManager(QObject *parent = nullptr);
    ~DataBaseManager();
    bool addUser(const QString& username);
    bool recordWin(int index);  // 胜场+1，总场+1，重新计算胜率
    bool recordLose(int index);
    QStringList getAllUsernames();
    QString getUserNameByIndex(int index);
    QList<UserRecord> getAllUsers();
    bool deleteUser(int id);
    bool updateUsername(int id, const QString& newUsername);
    bool isUsernameExists(const QString& username);
signals:
private:
    QSqlDatabase db;
    bool openDatabase();

};

#endif // DATABASEMANAGER_H
