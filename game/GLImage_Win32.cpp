//
//  Created by Kevin Wojniak on 2014/2/2.
//  Copyright (c) 2014 Kevin Wojniak. All rights reserved.
//

#include "GLImage.h"
#include <wincodec.h>
#include <shlwapi.h>

void GLImage::load(const unsigned char *buf, size_t bufSize)
{
    IStream *stream = SHCreateMemStream((const BYTE*)buf, (UINT)bufSize);
    if (stream != NULL) {
        IWICImagingFactory *pFactory;
        if (CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pFactory) == S_OK) {
            IWICBitmapDecoder *pDecoder;
            if (pFactory->CreateDecoderFromStream(stream, &CLSID_WICPngDecoder, WICDecodeMetadataCacheOnDemand, &pDecoder) == S_OK) {
                IWICBitmapFrameDecode *frame;
                if (pDecoder->GetFrame(0, &frame) == S_OK) {
                    UINT w, h;
                    if (frame->GetSize(&w, &h) == S_OK) {
                        width_ = w;
                        height_ = h;
                    }
                    IWICFormatConverter *formatConverter;
                    if (pFactory->CreateFormatConverter(&formatConverter) == S_OK) {
                        if (formatConverter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom) == S_OK) {
                            unsigned char *pixels = new unsigned char[w * h * 4];
                            if (formatConverter->CopyPixels(0, w * 4, w * h * 4, pixels) == S_OK) {
                                loadTextureData_(pixels);
                            }
                            delete[] pixels;
                        }
                        formatConverter->Release();
                    }
                }
                pDecoder->Release();
            }
            pFactory->Release();
        }
        stream->Release();
    }
}
