#include "GLSoundsQtImp.h"
#include <QDebug>

GL::SoundsQtImp::SoundsQtImp()
    : QObject(nullptr)
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
    audioOutput_ = new QAudioOutput(wave.format);
    audioOutput_->setBufferSize(static_cast<int>(wave.dataLen));
    connect(audioOutput_, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    buffer_ = new QBuffer(&bytes_);
    if (!buffer_->open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open buffer";
        return false;
    }
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
        if (audioOutput_->error() != QAudio::NoError) {
            qWarning() << "ACTIVE error occurred:" << audioOutput_->error();
        }
    } else if (state == QAudio::SuspendedState) {
        if (audioOutput_->error() != QAudio::NoError) {
            qWarning() << "SUSPEND error occurred:" << audioOutput_->error();
        }
    }
    if (state == QAudio::IdleState) {
        audioOutput_->stop();
        buffer_->close();
        if (audioOutput_->error() != QAudio::NoError) {
            qWarning() << "IDLE error occurred:" << audioOutput_->error();
        }
        disconnect(audioOutput_, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
        delete audioOutput_;
        audioOutput_ = nullptr;
        delete buffer_;
        buffer_ = nullptr;
        playing_ = false;
    } else if (state == QAudio::StoppedState) {
        if (audioOutput_->error() != QAudio::NoError) {
            qWarning() << "STOPPED error occurred:" << audioOutput_->error();
        }
    }
}
