#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
class DataBaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseManager(QObject *parent = nullptr);
    ~DataBaseManager();
    bool addUser(const QString& username);
    bool recordWin(const QString& username);  // 胜场+1，总场+1，重新计算胜率
    bool recordLose(const QString& username);
    QStringList getAllUsernames();
signals:
private:
    QSqlDatabase db;
    bool openDatabase();

};

#endif // DATABASEMANAGER_H
