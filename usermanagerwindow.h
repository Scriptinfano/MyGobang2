#ifndef USERMANAGERWINDOW_H
#define USERMANAGERWINDOW_H
#include<QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "databasemanager.h"

class UserManagerWindow:public QDialog
{
    Q_OBJECT
public:
    explicit UserManagerWindow(std::shared_ptr<DataBaseManager>dbManager,QWidget *parent = nullptr);
private slots:
    /**
     * @brief refreshUserTable 刷新用户表，重新调用数据库接口
     */
    void refreshUserTable();
    /**
     * @brief onAddUserClicked 添加用户按钮点击之后的响应函数
     */
    void onAddUserClicked();
    /**
     * @brief onDeleteUserClicked 删除用户按钮点击之后的响应函数
     */
    void onDeleteUserClicked();
    /**
     * @brief onEditUserClicked 编辑用户按钮点击之后的响应函数
     */
    void onEditUserClicked();
    /**
     * @brief onSelectionChanged
     */
    void onSelectionChanged();
private:
    void setupUI();
    void updateButtonStates();
    bool isUsernameExists(const QString& name);

    std::shared_ptr<DataBaseManager> databaseManager;
    QTableWidget* table;
    QPushButton* refreshButton;
    QPushButton* addUserButton;
    QPushButton* deleteUserButton;
    QPushButton* editUserButton;
};

#endif // USERMANAGERWINDOW_H
