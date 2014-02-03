//
//  Created by Kevin Wojniak on 8/4/13.
//  Copyright (c) 2013 Kevin Wojniak. All rights reserved.
//

#include "GLSounds.h"
#include "GLResources.h"
#include "GLUtils.h"
#if _WIN32
#include <windows.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include "GLBufferReader.h"
#endif

#if _WIN32
struct WaveData {
public:
    WAVEFORMATEX format;
    char *data;
    size_t dataLen;
    bool playing;
    WaveData()
        : data(nullptr)
        , dataLen(0)
        , playing(false)
    {
        ZeroMemory(&format, sizeof(format));
    }
};

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

bool aiffDataToWave(const uint8_t *data, unsigned dataLen, WaveData &output)
{
    GLBufferReader reader(data, dataLen);
    uint16_t numChannels = 0;
    uint32_t numSampleFrames = 0;
    uint16_t sampleSize = 0;
    uint32_t sampleRate = 0;
    size_t sampleDataLen = 0;
    size_t ssndChunkLen = 0;
    uint8_t *sampleData = nullptr;

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
            sampleData = new uint8_t[ssndChunkLen];
            if (reader.read(sampleData, ssndChunkLen) != ssndChunkLen) {
                delete[] sampleData;
                return false;
            }
        }
        if (!reader.seek(chunkOffset + chunkLen)) {
            break;
        }
    }

    if (numChannels == 0 || numSampleFrames == 0 || sampleSize == 0 || sampleRate == 0 || sampleData == nullptr) {
        return false;
    }

    // convert PCM samples from AIFF (0 - 255) to WAVE (-127 to +127)
    char *waveSampleData = new char[sampleDataLen];
    for (size_t i = 0; i < sampleDataLen; ++i) {
        unsigned char c = sampleData[i];
        waveSampleData[i] = c - 128;
    }
    delete[] sampleData;

    // Setup the structure used by the waveXxx functions
    output.format.wFormatTag = WAVE_FORMAT_PCM;
    output.format.nChannels = numChannels;
    output.format.wBitsPerSample = sampleSize;
    output.format.nAvgBytesPerSec = output.format.nSamplesPerSec = sampleRate * (sampleSize / 8);
    output.format.nBlockAlign = (output.format.nChannels * output.format.wBitsPerSample) / 8;
    output.data = waveSampleData;
    output.dataLen = sampleDataLen;

    return true;
}

// WaveOut is a C++ wrapper around the waveXxx functions.
class WaveOut {
public:
    WaveOut();
    bool play(const WaveData &wave);

private:
    bool open_;
    HWAVEOUT handle_;
    bool playing_;
    WAVEHDR header_;
    void doneCallback();
    friend void CALLBACK waveOutCallback(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
};

void CALLBACK waveOutCallback(HWAVEOUT wvHandle, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (uMsg == WOM_DONE) {
        ((WaveOut*)dwInstance)->doneCallback();
    }
}

WaveOut::WaveOut()
    : open_(false)
    , handle_(nullptr)
    , playing_(false)
{
}

bool WaveOut::play(const WaveData &wave)
{
    if (playing_) {
        return false;
    }
    MMRESULT res;
    if (!open_) {
        res = waveOutOpen(&handle_, WAVE_MAPPER, &wave.format, (DWORD_PTR)waveOutCallback, (DWORD_PTR)this, CALLBACK_FUNCTION);
        if (res != MMSYSERR_NOERROR) {
            GLUtils::log(L"waveOutOpen failed: %u\n", res);
            return false;
        }
        open_ = true;
    }
    ZeroMemory(&header_, sizeof(header_));
    header_.lpData = wave.data;
    header_.dwBufferLength = (DWORD)wave.dataLen;
    res = waveOutPrepareHeader(handle_, &header_, sizeof(header_));
    if (res != MMSYSERR_NOERROR) {
        GLUtils::log(L"waveOutPrepareHeader failed: %u\n", res);
        return false;
    }
    res = waveOutWrite(handle_, &header_, sizeof(header_));
    if (res != MMSYSERR_NOERROR) {
        GLUtils::log(L"waveOutWrite failed: %u\n", res);
        return false;
    }
    playing_ = true;
    return true;
}

void WaveOut::doneCallback()
{
    MMRESULT res = waveOutUnprepareHeader(handle_, &header_, sizeof(header_));
    if (res != MMSYSERR_NOERROR) {
        GLUtils::log(L"waveOutUnprepareHeader failed: %u\n", res);
    }
    playing_ = false;
}

class GLSounds::Imp {
public:
    Imp()
        : outs_(3) // up to 3 simultaneous sounds
    {
        loadSound(kBirdSound, bird_aif, bird_aif_len);
        loadSound(kFlapSound, flap_aif, flap_aif_len);
        loadSound(kGrateSound, grate_aif, grate_aif_len);
        loadSound(kWalkSound, walk_aif, walk_aif_len);
        loadSound(kScreechSound, screech_aif, screech_aif_len);
    }

    void play(int which) {
        bool didPlay = false;
        for (auto &out : outs_) {
            if (out.play(wavs_[which])) {
                didPlay = true;
                break;
            }
        }
        if (!didPlay) {
            GLUtils::log(L"Didn't play %d\n", which);
        }
    }

private:
    void loadSound(int which, const unsigned char *buf, unsigned bufLen) {
        if (!aiffDataToWave(buf, bufLen, wavs_[which])) {
            GLUtils::log(L"Can't load sound %d\n", which);
        }
    }

    WaveData wavs_[kMaxSounds];
    std::vector<WaveOut> outs_;
};
#endif

GLSounds::GLSounds()
#if _WIN32
    : imp(new Imp)
#endif
{
}

void GLSounds::play(int which)
{
#if _WIN32
    imp->play(which);
#endif
}
