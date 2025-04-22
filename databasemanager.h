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
    bool recordWin(int index);  // 胜场+1，总场+1，重新计算胜率
    bool recordLose(int index);
    QStringList getAllUsernames();
    QString getUserNameByIndex(int index);
signals:
private:
    QSqlDatabase db;
    bool openDatabase();

};

#endif // DATABASEMANAGER_H
