// MainMenu.cpp
#include "mainmenu.h"
#include "usermanagerwindow.h"
#include "videodialog.h"
#include "mainwindow.h"
MainMenu::MainMenu(QWidget *parent) : QWidget(parent) {
    setWindowTitle("五子棋");

    databaseManager=std::make_shared<DataBaseManager>();

    gameTypeLabel = std::make_unique<QLabel>("选择游戏模式:", this);
    gameTypeCombo = std::make_unique<QComboBox>(this);
    gameTypeCombo->addItem("人人对战");
    gameTypeCombo->addItem("人机对战");

    aiTypeLabel = std::make_unique<QLabel>("选择AI算法:", this);
    aiTypeCombo = std::make_unique<QComboBox>(this);
    aiTypeCombo->addItem("局部评估");
    aiTypeCombo->addItem("蒙特卡洛");
    aiTypeCombo->addItem("AlphaBeta");
    aiTypeCombo->setEnabled(false);

    userLabel=std::make_unique<QLabel>("选择用户",this);
    userCombo=std::make_unique<QComboBox>(this);
    QStringList names=databaseManager->getAllUsernames();
    userCombo->addItems(names);


    startButton =std::make_unique<QPushButton>("开始游戏", this);
    videoButton = std::make_unique<QPushButton>("播放算法讲解视频", this);
    userManageButton = std::make_unique<QPushButton>("用户管理",this);

    mainLayout = std::make_unique<QVBoxLayout>(this);

    gameTypeLayout = std::make_unique<QHBoxLayout>();
    gameTypeLayout->addWidget(gameTypeLabel.get());
    gameTypeLayout->addWidget(gameTypeCombo.get());

    aiTypeLayout = std::make_unique<QHBoxLayout>();
    aiTypeLayout->addWidget(aiTypeLabel.get());
    aiTypeLayout->addWidget(aiTypeCombo.get());

    userLayout= std::make_unique<QHBoxLayout>();
    userLayout->addWidget(userLabel.get());
    userLayout->addWidget(userCombo.get());

    mainLayout->addLayout(gameTypeLayout.get());
    mainLayout->addLayout(aiTypeLayout.get());
    mainLayout->addLayout(userLayout.get());
    mainLayout->addWidget(startButton.get());
    mainLayout->addWidget(videoButton.get());
    mainLayout->addWidget(userManageButton.get());

    connect(gameTypeCombo.get(), QOverload<int>::of(&QComboBox::currentIndexChanged),this, &MainMenu::onGameTypeChanged);
    connect(userCombo.get(), QOverload<int>::of(&QComboBox::currentIndexChanged),this, &MainMenu::onUserChanged);
    connect(startButton.get(), &QPushButton::clicked, this, &MainMenu::onStartClicked);
    connect(videoButton.get(), &QPushButton::clicked, this, &MainMenu::onPlayVideoClicked);
    connect(userManageButton.get(), &QPushButton::clicked, this, &MainMenu::onUserManageButtonClicked);
}

void MainMenu::onUserManageButtonClicked(){
    UserManagerWindow*userWindow=new UserManagerWindow(databaseManager,this);
    userWindow->setAttribute(Qt::WA_DeleteOnClose);
    userWindow->exec();
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
    int userIndex=userCombo->currentIndex();//combo中的第一个条目的编号从0开始，数据库中表users条目的编号从1开始
    currentUser=++userIndex;
    MainWindow *w=new MainWindow(databaseManager,gameType, aiType,currentUser,this);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->exec();
}
void MainMenu::onUserChanged(int index){
    qDebug()<<"index="<<index;
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

void MainMenu::onPlayVideoClicked() {
    VideoDialog *videoDialog = new VideoDialog(this);
    videoDialog->setAttribute(Qt::WA_DeleteOnClose);  // 关闭时自动销毁
    videoDialog->exec();
}
