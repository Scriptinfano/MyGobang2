#ifndef MAINMENU_H
#define MAINMENU_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "gamemodel.h"
#include "videodialog.h"
#include"databasemanager.h"
class MainMenu : public QDialog {
    Q_OBJECT
public:
    explicit MainMenu(std::shared_ptr<DataBaseManager>&theDatabaseManager,QWidget *parent = nullptr);

    GameType getGameType() const { return gameType; }

    AIType getAIType() const { return aiType; }

private slots:
    void onStartClicked();
    void onGameTypeChanged(int index);
    void onPlayVideoClicked();
private:
    std::unique_ptr<QLabel>gameTypeLabel;
    std::unique_ptr<QComboBox>gameTypeCombo;
    std::unique_ptr<QLabel>aiTypeLabel;
    std::unique_ptr<QComboBox>aiTypeCombo;
    std::unique_ptr<QLabel>userLabel;
    std::unique_ptr<QComboBox>userCombo;
    std::unique_ptr<QPushButton> startButton;
    std::unique_ptr<QPushButton>videoButton;
    std::unique_ptr<QVBoxLayout>mainLayout;
    std::unique_ptr<QHBoxLayout>gameTypeLayout;
    std::unique_ptr<QHBoxLayout>aiTypeLayout;
    std::unique_ptr<QHBoxLayout>userLayout;
    std::shared_ptr<DataBaseManager>databaseManager;
    GameType gameType = MAN;
    AIType aiType = LOCALEVALUATION;
};

#endif // MAINMENU_H
