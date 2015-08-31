#ifndef GLSOUNDSQTIMP_H
#define GLSOUNDSQTIMP_H

#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QAudioFormat>
#include <QAudioOutput>

namespace GL {

struct WaveData {
    QAudioFormat format;
    char *data;
    size_t dataLen;
    bool playing;
    WaveData()
        : data(0)
        , dataLen(0)
        , playing(false)
    {
    }

    WaveData(const WaveData& other) = delete;
};

class SoundsQtImp : public QObject {
    Q_OBJECT
public:
    explicit SoundsQtImp(QObject *parent = 0);

    bool play(const WaveData& wave);

private:
    SoundsQtImp(const WaveData& other) = delete;

    bool open_;
    QByteArray bytes_;
    QBuffer* buffer_;
    QAudioOutput* audioOutput_;
    bool playing_;
    void doneCallback();

private slots:
    void handleStateChanged(QAudio::State state);
};

} // namespace

#endif // GLSOUNDSQTIMP_H
