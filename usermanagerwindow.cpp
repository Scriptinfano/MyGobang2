#include "usermanagerwindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>

UserManagerWindow::UserManagerWindow(std::shared_ptr<DataBaseManager>dbManager,QWidget *parent) : QDialog(parent)
{
    databaseManager=dbManager;
    setupUI();
    refreshUserTable();
}

void UserManagerWindow::refreshUserTable()
{
    table->setRowCount(0);
    QList<UserRecord> users = databaseManager->getAllUsers();

    for (const UserRecord& user : users) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(user.id)));
        table->setItem(row, 1, new QTableWidgetItem(user.username));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(user.wins)));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(user.total)));
        QString winrateStr = QString::number(user.winrate * 100, 'f', 2) + "%";
        table->setItem(row, 4, new QTableWidgetItem(winrateStr));
    }

    updateButtonStates();
}

void UserManagerWindow::onAddUserClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("新建用户");
    QVBoxLayout layout(&dialog);
    QLineEdit* nameEdit = new QLineEdit;
    QLabel* infoLabel = new QLabel;

    layout.addWidget(new QLabel("请输入用户名:"));
    layout.addWidget(nameEdit);
    layout.addWidget(infoLabel);

    QPushButton* okButton = new QPushButton("确定");
    QPushButton* cancelButton = new QPushButton("取消");
    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(okButton);
    btnLayout->addWidget(cancelButton);
    layout.addLayout(btnLayout);

    connect(okButton, &QPushButton::clicked, [&] {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            infoLabel->setText("用户名不能为空");
        } else if (isUsernameExists(name)) {
            infoLabel->setText("用户名已存在，请重新输入");
        } else {
            databaseManager->addUser(name);
            dialog.accept();
            refreshUserTable();
        }
    });

    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    dialog.exec();
}

void UserManagerWindow::onDeleteUserClicked()
{
    QList<QTableWidgetItem*> selectedItems = table->selectedItems();
    QSet<int> selectedRows;
    for (QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }

    QList<int> idsToDelete;
    for (int row : selectedRows) {
        int id = table->item(row, 0)->text().toInt();
        idsToDelete.append(id);
    }

    bool success = true;
    for (int id : idsToDelete) {
        if (!databaseManager->deleteUser(id)) {
            success = false;
        }
    }

    if (success) {
        refreshUserTable();
    } else {
        QMessageBox::warning(this, "删除失败", "部分用户删除失败。");
    }
}

void UserManagerWindow::onEditUserClicked()
{
    QList<QTableWidgetItem*> selectedItems = table->selectedItems();
    if (selectedItems.isEmpty()) return;

    int row = selectedItems.first()->row();
    int id = table->item(row, 0)->text().toInt();
    QString oldName = table->item(row, 1)->text();

    bool ok;
    QString newName;
    do {
        newName = QInputDialog::getText(this, "编辑用户名", "请输入新用户名：", QLineEdit::Normal, oldName, &ok);
        if (!ok) return;
        if (newName == oldName) return;
        if (isUsernameExists(newName)) {
            QMessageBox::warning(this, "用户名已存在", "该用户名已存在，请输入其他名称。");
        } else {
            break;
        }
    } while (true);

    if (!databaseManager->updateUsername(id, newName)) {
        QMessageBox::warning(this, "更新失败", "用户名更新失败！");
    } else {
        refreshUserTable();
    }

}

void UserManagerWindow::onSelectionChanged()
{
    updateButtonStates();
}

void UserManagerWindow::setupUI()
{
    table = new QTableWidget(this);
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels(QStringList() << "ID" << "用户名" << "胜场" << "总场"<<"胜率");
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    table->horizontalHeader()->setStretchLastSection(true);

    refreshButton = new QPushButton("刷新");
    addUserButton = new QPushButton("新建用户");
    deleteUserButton = new QPushButton("删除用户");
    editUserButton = new QPushButton("编辑用户");

    deleteUserButton->setEnabled(false);
    editUserButton->setEnabled(false);

    connect(refreshButton, &QPushButton::clicked, this, &UserManagerWindow::refreshUserTable);
    connect(addUserButton, &QPushButton::clicked, this, &UserManagerWindow::onAddUserClicked);
    connect(deleteUserButton, &QPushButton::clicked, this, &UserManagerWindow::onDeleteUserClicked);
    connect(editUserButton, &QPushButton::clicked, this, &UserManagerWindow::onEditUserClicked);
    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &UserManagerWindow::onSelectionChanged);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(addUserButton);
    buttonLayout->addWidget(deleteUserButton);
    buttonLayout->addWidget(editUserButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(table);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void UserManagerWindow::updateButtonStates()
{
    int selectedCount = table->selectionModel()->selectedRows().count();
    deleteUserButton->setEnabled(selectedCount >= 1);
    editUserButton->setEnabled(selectedCount == 1);
}

bool UserManagerWindow::isUsernameExists(const QString &name)
{
    return databaseManager->isUsernameExists(name);
}
