#ifndef MAINMENU_H
#define MAINMENU_H

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "gamemodel.h"

#include "databasemanager.h"
class MainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr);

private slots:
    /**
     * 开始游戏按钮被点击
     */
    void onStartClicked();
    /**
     * 切换游戏类型的时候触发
     */
    void onGameTypeChanged(int index);
    /**
     * 切换主AI类型的时候触发
     */
    void onAiTypeChanged(int index);
    /**
     * 切换用户的时候触发
     */
    void onUserChanged(int index);
    /**
     * 进入视频播放界面
     */
    void onPlayVideoClicked();
    /**
     * 进入用户管理界面
     */
    void onUserManageButtonClicked();
    /**
     * 用户点击userCombo的时候从数据库中重新读取数据
     */
    void onUserComboBoxClicked();

private:
    std::unique_ptr<QLabel> gameTypeLabel;
    std::unique_ptr<QComboBox> gameTypeCombo;
    std::unique_ptr<QHBoxLayout> gameTypeLayout;

    std::unique_ptr<QLabel> aiTypeLabel;
    std::unique_ptr<QComboBox> aiTypeCombo;
    std::unique_ptr<QHBoxLayout> aiTypeLayout;

    std::unique_ptr<QLabel> otherAiTypeLabel;
    std::unique_ptr<QComboBox> otherAiTypeCombo;
    std::unique_ptr<QHBoxLayout> otherAiTypeLayout;

    std::unique_ptr<QLabel> userLabel;
    std::unique_ptr<QComboBox> userCombo;
    std::unique_ptr<QHBoxLayout> userLayout;

    std::unique_ptr<QLabel> difficultyLabel;
    std::unique_ptr<QComboBox> difficultyCombo;
    std::unique_ptr<QHBoxLayout> difficultyLayout;

    std::unique_ptr<QPushButton> startButton;
    std::unique_ptr<QPushButton> videoButton;

    std::unique_ptr<QVBoxLayout> mainLayout;

    std::unique_ptr<QPushButton> userManageButton;

    std::shared_ptr<DataBaseManager> databaseManager;

    GameType gameType = MAN;
    AIType aiType = LOCALEVALUATION;
    AIType otherAiType = LOCALEVALUATION;
    int currentUser = 1; // 当前用户在数据库表中条目的编号

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};

#endif // MAINMENU_H
