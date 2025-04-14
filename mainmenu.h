#ifndef MAINMENU_H
#define MAINMENU_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include "gamemodel.h"

class MainMenu : public QDialog {
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr);

    GameType getGameType() const { return gameType; }

    AIType getAIType() const { return aiType; }

private slots:
    void onStartClicked();
    void onGameTypeChanged(int index);

private:
    QComboBox *gameTypeCombo;
    QComboBox *aiTypeCombo;
    QPushButton *startButton;
    GameType gameType = MAN;
    AIType aiType = LOCALEVALUATION;
};

#endif // MAINMENU_H
