// MainMenu.cpp
#include "mainmenu.h"
#include "usermanagerwindow.h"
#include "videodialog.h"
#include "mainwindow.h"
#include <QEvent>
#include <QCloseEvent>

MainMenu::MainMenu(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("五子棋");

    databaseManager = std::make_shared<DataBaseManager>();

    gameTypeLabel = std::make_unique<QLabel>("选择游戏模式:", this);
    gameTypeCombo = std::make_unique<QComboBox>(this);
    gameTypeCombo->addItem("人人对战");
    gameTypeCombo->addItem("人机对战");
    gameTypeCombo->addItem("AI相互对战");

    aiTypeLabel = std::make_unique<QLabel>("选择AI算法:", this);
    aiTypeCombo = std::make_unique<QComboBox>(this);
    aiTypeCombo->addItem("局部评估");
    aiTypeCombo->addItem("蒙特卡洛");
    aiTypeCombo->addItem("AlphaBeta");
    aiTypeCombo->setEnabled(false);

    otherAiTypeLabel = std::make_unique<QLabel>("选择AI要挑战的另一个AI", this);
    otherAiTypeCombo = std::make_unique<QComboBox>(this);
    otherAiTypeCombo->addItem("局部评估");
    otherAiTypeCombo->addItem("蒙特卡洛");
    otherAiTypeCombo->addItem("AlphaBeta");
    otherAiTypeCombo->setEnabled(false);

    difficultyLabel = std::make_unique<QLabel>("选择难度等级", this);
    difficultyCombo = std::make_unique<QComboBox>(this);
    difficultyCombo->addItem("初级");
    difficultyCombo->addItem("中级");
    difficultyCombo->addItem("高级");
    difficultyCombo->setEnabled(false);

    userLabel = std::make_unique<QLabel>("选择用户", this);
    userCombo = std::make_unique<QComboBox>(this);
    QStringList names = databaseManager->getAllUsernames();
    userCombo->addItems(names);
    userCombo->installEventFilter(this);
    userCombo->setEnabled(false);

    startButton = std::make_unique<QPushButton>("开始游戏", this);
    videoButton = std::make_unique<QPushButton>("播放算法讲解视频", this);
    userManageButton = std::make_unique<QPushButton>("用户管理", this);

    mainLayout = std::make_unique<QVBoxLayout>(this);

    gameTypeLayout = std::make_unique<QHBoxLayout>();
    gameTypeLayout->addWidget(gameTypeLabel.get());
    gameTypeLayout->addWidget(gameTypeCombo.get());

    aiTypeLayout = std::make_unique<QHBoxLayout>();
    aiTypeLayout->addWidget(aiTypeLabel.get());
    aiTypeLayout->addWidget(aiTypeCombo.get());

    userLayout = std::make_unique<QHBoxLayout>();
    userLayout->addWidget(userLabel.get());
    userLayout->addWidget(userCombo.get());

    otherAiTypeLayout = std::make_unique<QHBoxLayout>();
    otherAiTypeLayout->addWidget(otherAiTypeLabel.get());
    otherAiTypeLayout->addWidget(otherAiTypeCombo.get());

    difficultyLayout = std::make_unique<QHBoxLayout>();
    difficultyLayout->addWidget(difficultyLabel.get());
    difficultyLayout->addWidget(difficultyCombo.get());

    mainLayout->addLayout(gameTypeLayout.get());
    mainLayout->addLayout(aiTypeLayout.get());
    mainLayout->addLayout(otherAiTypeLayout.get());
    mainLayout->addLayout(difficultyLayout.get());
    mainLayout->addLayout(userLayout.get());
    mainLayout->addWidget(startButton.get());
    mainLayout->addWidget(videoButton.get());
    mainLayout->addWidget(userManageButton.get());

    connect(gameTypeCombo.get(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainMenu::onGameTypeChanged);
    connect(difficultyCombo.get(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainMenu::onDifficultyChanged);
    connect(userCombo.get(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainMenu::onUserChanged);
    connect(startButton.get(), &QPushButton::clicked, this, &MainMenu::onStartClicked);
    connect(videoButton.get(), &QPushButton::clicked, this, &MainMenu::onPlayVideoClicked);
    connect(userManageButton.get(), &QPushButton::clicked, this, &MainMenu::onUserManageButtonClicked);
    connect(aiTypeCombo.get(), QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainMenu::onAiTypeChanged);
}
void MainMenu::onDifficultyChanged(int index)
{
    switch (index)
    {
        case 0:
        {
            
            break;
        }

        case 1:
        {
            
            break;
        }
        case 2:
        {


            break;
        }
    }
}
void MainMenu::onUserComboBoxClicked()
{
    // 重新从数据库中读取数据
    qDebug() << "onUserComboBoxClicked触发";
    QStringList names = databaseManager->getAllUsernames();
    userCombo->clear();
    userCombo->addItems(names);
}
bool MainMenu::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == userCombo.get() && event->type() == QEvent::MouseButtonPress)
    {
        onUserComboBoxClicked();
        return false; // 继续传递事件
    }
    return QWidget::eventFilter(obj, event);
}

void MainMenu::onUserManageButtonClicked()
{
    UserManagerWindow *userWindow = new UserManagerWindow(databaseManager, this);
    userWindow->setAttribute(Qt::WA_DeleteOnClose);
    userWindow->exec();
}

void MainMenu::onStartClicked()
{
    if (gameType == AI || gameType == AIAI)
    {
        int curindex = aiTypeCombo->currentIndex();
        if (curindex == 0)
            aiType = LOCALEVALUATION;
        else if (curindex == 1)
            aiType = MCTS;
        else if (curindex == 2)
        {
            aiType = ALPHABETA;
            // TODO 在算法是ALPHABETA且选择人机对战的情况下，将难度等级传入MainWindow
        }
        else
            throw std::runtime_error("aiType出现了未定义的行为");
        if (gameType == AIAI)
        {
            curindex = otherAiTypeCombo->currentIndex();
            if (curindex == 0)
                otherAiType = LOCALEVALUATION;
            else if (curindex == 1)
                otherAiType = MCTS;
            else if (curindex == 2)
                otherAiType = ALPHABETA;
            else
                throw std::runtime_error("otherAiType出现了未定义的行为");
        }
    }
    int userIndex = userCombo->currentIndex(); // combo中的第一个条目的编号从0开始，数据库中表users条目的编号从1开始
    currentUser = ++userIndex;
    // TODO 改造这里的MainWindow函数调用，来接受一个难度等级参数
    MainWindow *w = new MainWindow(databaseManager, gameType, aiType, otherAiType, currentUser, this);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->exec();
}
void MainMenu::onUserChanged(int index)
{
    qDebug() << "onUserChanged()触发，index=" << index;
}
void MainMenu::onGameTypeChanged(int index)
{
    if (index == 1)
    { // 选择了人机对战
        aiTypeCombo->setEnabled(true);
        otherAiTypeCombo->setEnabled(false);
        userCombo->setEnabled(true);
        difficultyCombo->setEnabled(true);
        gameType = AI;
        int curindex = aiTypeCombo->currentIndex();
        // 只有在选择了人机对战且选择alpha-beta算法的情况下才能调节难度
        if (curindex != 2)
            difficultyCombo->setEnabled(false);
        else
            difficultyCombo->setEnabled(true);
    }
    else if (index == 0)
    { // 人人对战
        aiTypeCombo->setEnabled(false);
        otherAiTypeCombo->setEnabled(false);
        userCombo->setEnabled(false);
        difficultyCombo->setEnabled(false);
        gameType = MAN;
    }
    else
    {
        // AI与AI对战
        otherAiTypeCombo->setEnabled(true);
        aiTypeCombo->setEnabled(true);
        // 用户复选框强行锁定到robots
        userCombo->setCurrentIndex(1);
        userCombo->setEnabled(false);
        difficultyCombo->setEnabled(false);
        gameType = AIAI;
    }
}
void MainMenu::onAiTypeChanged(int index)
{
    switch (index)
    {
    case 0:
    {
        difficultyCombo->setEnabled(false);
        break;
    }

    case 1:
    {
        difficultyCombo->setEnabled(false);
        break;
    }
    case 2:
    {
        if (gameTypeCombo->currentIndex() != 2)
            difficultyCombo->setEnabled(true);
        break;
    }
    }
}
void MainMenu::onPlayVideoClicked()
{
    VideoDialog *videoDialog = new VideoDialog(this);
    videoDialog->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动销毁
    videoDialog->exec();
}

void MainMenu::closeEvent(QCloseEvent *event)
{
    event->accept();            // 接受关闭事件
    QWidget::closeEvent(event); // 调用父类的 closeEvent
}
