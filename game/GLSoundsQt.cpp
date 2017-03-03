#include "GLSounds.h"
#include "GLUtils.h"
#include <stdint.h>
#include <cstring>
#include <memory>
#include <vector>
#include "GLBufferReader.h"
#include "GLSoundsQtImp.h"
#include <QDebug>
#include <QAudioDeviceInfo>

uint32_t read32LittleEndian(const uint8_t *data)
{
    uint32_t val = 0;
    val |= (uint32_t)data[0];
    val |= (uint32_t)data[1] << 8;
    val |= (uint32_t)data[2] << 16;
    val |= (uint32_t)data[3] << 24;
    return val;
}

uint16_t read16LittleEndian(const uint8_t *data)
{
    uint16_t val = 0;
    val |= (uint16_t)data[0];
    val |= (uint16_t)data[1] << 8;
    return val;
}

bool dataToWave(const uint8_t *data, unsigned dataLen, GL::WaveData &output)
{
    GLBufferReader reader(data, dataLen);
    uint16_t numChannels = 0;
    uint32_t numSampleFrames = 0;
    uint16_t sampleSize = 0;
    uint32_t sampleRate = 0;
    size_t sampleDataLen = 0;
    char *sampleData = nullptr;

    // Check header
    const uint8_t fourccRiff[4] = { 'R', 'I', 'F', 'F' };
    const uint8_t fourccWave[4] = { 'W', 'A', 'V', 'E' };
    uint8_t header[12];
    if (reader.read(header, sizeof(header)) != sizeof(header) ||
        std::memcmp(header, fourccRiff, sizeof(fourccRiff)) != 0 ||
        std::memcmp(header + 8, fourccWave, sizeof(fourccWave)) != 0) {
        return false;
    }

    // Read chunks
    const uint8_t chunkFmt[4] = { 'f', 'm', 't', ' ' };
    const uint8_t chunkData[4] = { 'd', 'a', 't', 'a' };
    uint8_t chunkHeader[8];
    while (reader.read(chunkHeader, sizeof(chunkHeader)) == sizeof(chunkHeader)) {
        uint32_t chunkLen = read32LittleEndian(chunkHeader + 4);
        size_t chunkOffset = reader.offset();
        if (std::memcmp(chunkHeader, chunkFmt, 4) == 0) {
            uint8_t commData[16];
            if (chunkLen != sizeof(commData) || reader.read(commData, sizeof(commData)) != sizeof(commData)) {
                return false;
            }
            int format = read16LittleEndian(commData);
            numChannels = read16LittleEndian(commData + 2);
            sampleRate = read32LittleEndian(commData + 4);
            sampleSize = read16LittleEndian(commData + 14);
            if (format != 1 || numChannels != 1 || sampleSize != 8) {
                // This code only supports 8-bit mono.
                return false;
            }
        } else if (std::memcmp(chunkHeader, chunkData, 4) == 0) {
            numSampleFrames = chunkLen;
            sampleDataLen = (sampleSize / 8) * numSampleFrames * numChannels;
            sampleData = new char[chunkLen];
            if (reader.read((uint8_t*)sampleData, chunkLen) != chunkLen) {
                return false;
            }
        }
        if (!reader.seek(chunkOffset + chunkLen)) {
            break;
        }
    }

    if (numChannels == 0 || numSampleFrames == 0 || sampleSize == 0 || sampleRate == 0 || sampleData == NULL) {
        return false;
    }

    output.format.setCodec("audio/pcm");
    output.format.setChannelCount(numChannels);
    output.format.setSampleSize(sampleSize);
    output.format.setSampleRate(sampleRate);
    output.format.setSampleType(QAudioFormat::UnSignedInt);

    if (!output.format.isValid()) {
        qWarning() << "Audio format is invalid.";
        return false;
    }

    const QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(output.format)) {
        qWarning() << "Audio format is not supported.";
        return false;
    }

    output.data = sampleData;
    output.dataLen = sampleDataLen;

    return true;
}

struct Context {
    GL::WaveData wavs[kMaxSounds];
    std::vector<GL::SoundsQtImp*> outs;
};

void GL::Sounds::initContext()
{
    context = new Context;
}

void GL::Sounds::play(int which) {
    Context *ctx = static_cast<Context*>(context);
    bool didPlay = false;
    const GL::WaveData& data = ctx->wavs[which];
    for (auto& out : ctx->outs) {
        if (out->play(data)) {
            didPlay = true;
            break;
        }
    }
    if (!didPlay) {
        GL::SoundsQtImp* imp = new GL::SoundsQtImp;
        imp->play(data);
        ctx->outs.push_back(imp);
    }
}

void GL::Sounds::load(int which, const unsigned char *buf, unsigned bufLen) {
    Context *ctx = static_cast<Context*>(context);
    if (!dataToWave(buf, bufLen, ctx->wavs[which])) {
        qWarning("Can't load sound %d", which);
    }
}
