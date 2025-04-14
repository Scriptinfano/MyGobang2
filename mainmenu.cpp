// MainMenu.cpp
#include "mainmenu.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
MainMenu::MainMenu(QWidget *parent) : QDialog(parent) {
    setWindowTitle("五子棋");

    QLabel *gameTypeLabel = new QLabel("选择游戏模式:", this);
    gameTypeCombo = new QComboBox(this);
    gameTypeCombo->addItem("人人对战");
    gameTypeCombo->addItem("人机对战");

    QLabel *aiTypeLabel = new QLabel("选择AI算法:", this);
    aiTypeCombo = new QComboBox(this);
    aiTypeCombo->addItem("局部评估");
    aiTypeCombo->addItem("蒙特卡洛");
    aiTypeCombo->addItem("AlphaBeta");
    aiTypeCombo->setEnabled(false);

    startButton = new QPushButton("开始游戏", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *gameTypeLayout = new QHBoxLayout;
    gameTypeLayout->addWidget(gameTypeLabel);
    gameTypeLayout->addWidget(gameTypeCombo);

    QHBoxLayout *aiTypeLayout = new QHBoxLayout;
    aiTypeLayout->addWidget(aiTypeLabel);
    aiTypeLayout->addWidget(aiTypeCombo);

    mainLayout->addLayout(gameTypeLayout);
    mainLayout->addLayout(aiTypeLayout);
    mainLayout->addWidget(startButton);

    connect(gameTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainMenu::onGameTypeChanged);
    connect(startButton, &QPushButton::clicked, this, &MainMenu::onStartClicked);
}

void MainMenu::onStartClicked() {
    if(gameType==AI){
        int curindex=aiTypeCombo->currentIndex();
        if(curindex==0)
            aiType=LOCALEVALUATION;
        else if(curindex==1)
            aiType=MCTS;
        else if(curindex==2)
            aiType=ALPHABETA;
        else
            throw std::runtime_error("aiType出现了未定义的行为");
    }
    accept();
}

void MainMenu::onGameTypeChanged(int index) {
    if (index == 1) { // 选择了人机对战
        aiTypeCombo->setEnabled(true);
        gameType=AI;
    } else { // 人人对战
        aiTypeCombo->setEnabled(false);
        gameType=MAN;
    }
}
