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

uint32_t read32BigEndian(const uint8_t *data)
{
    uint32_t val = 0;
    val |= (uint32_t)data[3];
    val |= (uint32_t)data[2] << 8;
    val |= (uint32_t)data[1] << 16;
    val |= (uint32_t)data[0] << 24;
    return val;
}

uint16_t read16BigEndian(const uint8_t *data)
{
    uint16_t val = 0;
    val |= (uint16_t)data[1];
    val |= (uint16_t)data[0] << 8;
    return val;
}

// Converts an 80 bit IEEE Standard 754 floating point number to an unsigned long.
uint32_t read80Float(const uint8_t *data)
{
    uint32_t mantissa = read32BigEndian(data + 2);
    uint8_t exp = 30 - *(data + 1);
    uint32_t last = 0;
    while (exp--) {
        last = mantissa;
        mantissa >>= 1;
    }
    if (last & 1) {
        mantissa++;
    }
    return mantissa;
}

bool aiffDataToWave(const uint8_t *data, unsigned dataLen, GL::WaveData &output)
{
    GLBufferReader reader(data, dataLen);
    uint16_t numChannels = 0;
    uint32_t numSampleFrames = 0;
    uint16_t sampleSize = 0;
    uint32_t sampleRate = 0;
    size_t sampleDataLen = 0;
    size_t ssndChunkLen = 0;
    std::unique_ptr<uint8_t[]> sampleData;

    // Check header
    const uint8_t fourccForm[4] = { 'F', 'O', 'R', 'M' };
    const uint8_t fourccAiff[4] = { 'A', 'I', 'F', 'F' };
    uint8_t header[12];
    if (reader.read(header, sizeof(header)) != sizeof(header) ||
        std::memcmp(header, fourccForm, sizeof(fourccForm)) != 0 ||
        std::memcmp(header + 8, fourccAiff, sizeof(fourccAiff)) != 0) {
        return false;
    }

    // Read chunks
    const uint8_t fourccChunkComm[4] = { 'C', 'O', 'M', 'M' };
    const uint8_t fourccChunkSsnd[4] = { 'S', 'S', 'N', 'D' };
    uint8_t chunkHeader[8];
    while (reader.read(chunkHeader, sizeof(chunkHeader)) == sizeof(chunkHeader)) {
        uint32_t chunkLen = read32BigEndian(chunkHeader + 4);
        size_t chunkOffset = reader.offset();
        if (std::memcmp(chunkHeader, fourccChunkComm, 4) == 0) {
            uint8_t commData[18];
            if (chunkLen != sizeof(commData) || reader.read(commData, sizeof(commData)) != sizeof(commData)) {
                return false;
            }
            numChannels = read16BigEndian(commData);
            numSampleFrames = read32BigEndian(commData + 2);
            sampleSize = read16BigEndian(commData + 6);
            sampleRate = read80Float(commData + 8);
            if (sampleSize != 8 || numChannels != 1) {
                // This code only supports 8-bit mono.
                return false;
            }
        } else if (std::memcmp(chunkHeader, fourccChunkSsnd, 4) == 0) {
            // We make the assumption that the SSND chunk comes after the COMM chunk. This isn't required for AIFF files though.
            sampleDataLen = (sampleSize / 8) * numSampleFrames * numChannels;
            ssndChunkLen = sampleDataLen + 8;
            if (chunkLen != ssndChunkLen) {
                return false;
            }
            sampleData.reset(new uint8_t[ssndChunkLen]);
            if (reader.read(sampleData.get(), ssndChunkLen) != ssndChunkLen) {
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

    // convert PCM samples from AIFF (0 - 255) to WAVE (-127 to +127)
    char *waveSampleData = new char[sampleDataLen];
    for (size_t i = 0; i < sampleDataLen; ++i) {
        unsigned char c = sampleData[i];
        waveSampleData[i] = c - 128;
    }

    output.format.setCodec("audio/pcm");
    output.format.setChannelCount(numChannels);
    output.format.setSampleSize(sampleSize);
    output.format.setSampleRate(sampleRate);
#ifdef __APPLE__
    output.format.setSampleType(QAudioFormat::UnSignedInt);
#else
    output.format.setSampleType(QAudioFormat::SignedInt);
#endif

    if (!output.format.isValid()) {
        qWarning() << "Audio format is invalid.";
        return false;
    }

    const QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(output.format)) {
        qWarning() << "Audio format is not supported.";
        return false;
    }

    output.data = waveSampleData;
    output.dataLen = sampleDataLen;

    return true;
}

struct Context {
    GL::WaveData wavs[kMaxSounds];
    std::vector<GL::SoundsQtImp> outs;
    Context()
        : outs(10)
    {}
};

void GL::Sounds::initContext()
{
    context = new Context;
}

void GL::Sounds::play(int which) {
    Context *ctx = static_cast<Context*>(context);
    bool didPlay = false;
    for (auto& out : ctx->outs) {
        if (out.play(ctx->wavs[which])) {
            didPlay = true;
            break;
        }
    }
    if (!didPlay) {
        qWarning("Didn't play %d", which);
    }
}

void GL::Sounds::load(int which, const unsigned char *buf, unsigned bufLen) {
    Context *ctx = static_cast<Context*>(context);
    if (!aiffDataToWave(buf, bufLen, ctx->wavs[which])) {
        qWarning("Can't load sound %d", which);
    } else {
        char path[1024];
        snprintf(path, sizeof(path), "/Users/kainjow/Desktop/%d.wav", which);
        FILE *f = fopen(path, "wb");
        if (!f) {
            printf("Can't open file: %s\n", path);
        } else {
            if (fwrite(ctx->wavs[which].data, 1, ctx->wavs[which].dataLen, f) != ctx->wavs[which].dataLen) {
                printf("Can't write file\n");
            }
            fclose(f);
        }
    }
}
