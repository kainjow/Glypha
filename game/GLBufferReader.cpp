#include "GLBufferReader.h"
#include <string.h>

GLBufferReader::GLBufferReader(const uint8_t *data, size_t dataLen)
    : data_(data)
    , dataLen_(dataLen)
    , offset_(0)
{
}

size_t GLBufferReader::read(uint8_t *data, size_t count)
{
    if (offset_ + count > dataLen_) {
        count = dataLen_ - offset_;
    }
    ::memcpy(data, data_ + offset_, count);
    offset_ += count;
    return count;
}

bool GLBufferReader::seek(size_t offset)
{
    if (offset > dataLen_) {
        return false;
    }
    offset_ = offset;
    return true;
}

size_t GLBufferReader::offset() const
{
    return offset_;
}
