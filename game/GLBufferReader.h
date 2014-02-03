//
//  Created by Kevin Wojniak on 2014/2/2.
//  Copyright (c) 2014 Kevin Wojniak. All rights reserved.
//

#ifndef GL_BUFFER_READER_H
#define GL_BUFFER_READER_H

#include <stdint.h>
#include <stddef.h>

class GLBufferReader {
public:
    GLBufferReader(const uint8_t *data, size_t dataLen);
    size_t read(uint8_t *data, size_t count);
    bool seek(size_t offset);
    size_t offset();
private:
    const uint8_t *data_;
    size_t dataLen_;
    size_t offset_;
};

#endif