// VideoDialog.cpp
#include "videodialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QLabel>
#include <QAudioOutput>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QCoreApplication>
VideoDialog::VideoDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("播放算法讲解视频");
    resize(800, 600);

    player = std::make_unique<QMediaPlayer>(this);
    videoWidget = std::make_unique<QVideoWidget>(this);
    audioOutput=std::make_unique<QAudioOutput>(this);
    videoSelector =std::make_unique<QComboBox>(this);
    playButton =std::make_unique<QPushButton>("播放", this);
    pauseButton = std::make_unique<QPushButton>("暂停", this);
    slider = std::make_unique<QSlider>(Qt::Horizontal, this);

    player->setVideoOutput(videoWidget.get());
    player->setAudioOutput(audioOutput.get());

    QString videoPath = QCoreApplication::applicationDirPath() + "/../Resources/videos";
    QDir videoDir(videoPath);

    QStringList filters;
    filters << "*.mp4" << "*.avi" << "*.mov";
    QFileInfoList videoList = videoDir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo &fileInfo : videoList) {
        videoSelector->addItem(fileInfo.fileName(), fileInfo.absoluteFilePath());
    }
    if (!videoSelector->currentText().isEmpty()) {
        onVideoSelected(videoSelector->currentIndex());
    }

    connect(videoSelector.get(), QOverload<int>::of(&QComboBox::currentIndexChanged),this, &VideoDialog::onVideoSelected);
    connect(playButton.get(), &QPushButton::clicked, this, &VideoDialog::play);
    connect(pauseButton.get(), &QPushButton::clicked, this, &VideoDialog::pause);


    slider->setRange(0, 0);
    connect(slider.get(), &QSlider::sliderMoved, this, &VideoDialog::setPosition);
    connect(player.get(), &QMediaPlayer::positionChanged, this, &VideoDialog::updateSlider);
    connect(player.get(), &QMediaPlayer::durationChanged, [this](qint64 duration){
        slider->setRange(0, duration);
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(videoSelector.get());
    layout->addWidget(videoWidget.get());

    QHBoxLayout *controls = new QHBoxLayout;
    controls->addWidget(playButton.get());
    controls->addWidget(pauseButton.get());

    layout->addLayout(controls);    // 控制按钮
    layout->addWidget(slider.get());      // 播放进度条（单独一行）
}

VideoDialog::~VideoDialog()
{

}

void VideoDialog::onVideoSelected(int index) {
    QString filePath = videoSelector->itemData(index).toString();
    player->setSource(QUrl::fromLocalFile(filePath));
    player->play();
}

void VideoDialog::play() {
    player->play();
}

void VideoDialog::pause() {
    player->pause();
}

void VideoDialog::updateSlider(qint64 position) {
    slider->setValue(position);
}

void VideoDialog::setPosition(int position) {
    player->setPosition(position);
}
