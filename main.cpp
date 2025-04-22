#include "mainwindow.h"
#include "mainmenu.h"
#include <QApplication>
#include "databasemanager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::shared_ptr<DataBaseManager>databaseManager=std::make_shared<DataBaseManager>();

    MainMenu menu(databaseManager);
    if (menu.exec() == QDialog::Accepted) {
        MainWindow w(databaseManager,menu.getGameType(), menu.getAIType());
        w.show();
        return a.exec();
    }
}
