#include "GLSoundsQtImp.h"
#include <QDebug>

GL::SoundsQtImp::SoundsQtImp(QObject *parent)
    : QObject(parent)
    , open_(false)
    , buffer_(nullptr)
    , audioOutput_(nullptr)
    , playing_(false)
{
}

bool GL::SoundsQtImp::play(const WaveData &wave)
{
    if (playing_) {
        return false;
    }
    bytes_ = QByteArray(wave.data, wave.dataLen);
    buffer_ = new QBuffer(&bytes_);
    if (!buffer_->open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open buffer";
        return false;
    }
    audioOutput_ = new QAudioOutput(wave.format);
    connect(audioOutput_, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    audioOutput_->start(buffer_);
    if (audioOutput_->error() != QAudio::NoError) {
        qWarning() << "START error occurred:" << audioOutput_->error();
    }
    playing_ = true;
    return true;
}

void GL::SoundsQtImp::handleStateChanged(QAudio::State state)
{
    if (state == QAudio::ActiveState) {
        qDebug() << "WENT ACTIVE!";
        if (audioOutput_ && audioOutput_->error() != QAudio::NoError) {
            qWarning() << "ACTIVE error occurred:" << audioOutput_->error();
        }
    } else if (state == QAudio::SuspendedState) {
        qDebug() << "WENT SUSPEND!";
        if (audioOutput_ && audioOutput_->error() != QAudio::NoError) {
            qWarning() << "SUSPEND error occurred:" << audioOutput_->error();
        }
    }
    if (state == QAudio::IdleState) {
        qDebug() << "WENT IDLE!";
        if (audioOutput_) {
            if (audioOutput_->error() != QAudio::NoError) {
                qWarning() << "IDLE error occurred:" << audioOutput_->error();
            }
            audioOutput_->stop();
        }
        if (buffer_) {
            buffer_->close();
        }
    } else if (state == QAudio::StoppedState) {
        qDebug() << "WENT STOPPED!";
        if (audioOutput_) {
            if (audioOutput_->error() != QAudio::NoError) {
                qWarning() << "STOPPED error occurred:" << audioOutput_->error();
            }
            disconnect(audioOutput_, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
            delete audioOutput_;
            audioOutput_ = nullptr;
        }
        if (buffer_) {
            delete buffer_;
            buffer_ = nullptr;
        }
        playing_ = false;
    }
}
