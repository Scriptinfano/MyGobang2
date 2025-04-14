#include "mainwindow.h"
#include "mainmenu.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainMenu menu;
    if (menu.exec() == QDialog::Accepted) {
        MainWindow w(menu.getGameType(), menu.getAIType());
        w.show();
        return a.exec();
    }
}
