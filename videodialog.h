#ifndef VIDEODIALOG_H
#define VIDEODIALOG_H

#include <QDialog>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QAudioOutput>
#include <memory>
class VideoDialog : public QDialog {
    Q_OBJECT
public:
    explicit VideoDialog(QWidget *parent = nullptr);
    ~VideoDialog();
private slots:
    void onVideoSelected(int index);
    void play();
    void pause();
    void updateSlider(qint64 position);
    void setPosition(int position);

private:
    std::unique_ptr<QComboBox>videoSelector;
    std::unique_ptr<QMediaPlayer>player;
    std::unique_ptr<QVideoWidget>videoWidget;
    std::unique_ptr<QAudioOutput>audioOutput;
    std::unique_ptr<QPushButton>playButton;
    std::unique_ptr<QPushButton>pauseButton;
    std::unique_ptr<QSlider>slider;
};

#endif // VIDEODIALOG_H
