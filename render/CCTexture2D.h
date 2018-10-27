/****************************************************************************
Copyright (c) 2008      Apple Inc. All Rights Reserved.
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCTEXTURE2D_H__
#define __CCTEXTURE2D_H__

#include <string>
#include <map>
#include "CommonMacros.h"
#include "PlatformMacros.h"
#include "CCGeometry.h"

typedef struct _MipmapInfo MipmapInfo;

/**
 * @addtogroup textures
 * @{
 */

class Image;
//CONSTANTS:

class GLProgram;

//CLASS INTERFACES:

/** @brief Texture2D class.
* This class allows to easily create OpenGL 2D textures from images, text or raw data.
* The created Texture2D object will always have power-of-two dimensions.
* Depending on how you create the Texture2D object, the actual image area of the texture might be smaller than the texture dimensions i.e. "contentSize" != (pixelsWide, pixelsHigh) and (maxS, maxT) != (1.0, 1.0).
* Be aware that the content of the generated textures will be upside-down!
*/
class  Texture2D
#ifdef EMSCRIPTEN
, public GLBufferedNode
#endif // EMSCRIPTEN
{
public:
    /** @typedef Texture2D::PixelFormat
     Possible texture pixel formats
     */
    enum class PixelFormat
    {
        //! auto detect the type
        AUTO,
        //! 32-bit texture: BGRA8888
        BGRA8888,
        //! 32-bit texture: RGBA8888
        RGBA8888,
        //! 24-bit texture: RGBA888
        RGB888,
        //! 16-bit texture without Alpha channel
        RGB565,
        //! 8-bit textures used as masks
        A8,
        //! 8-bit intensity texture
        I8,
        //! 16-bit textures used as masks
        AI88,
        //! 16-bit textures: RGBA4444
        RGBA4444,
        //! 16-bit textures: RGB5A1
        RGB5A1,
        //! 4-bit PVRTC-compressed texture: PVRTC4
        PVRTC4,
        //! 4-bit PVRTC-compressed texture: PVRTC4 (has alpha channel)
        PVRTC4A,
        //! 2-bit PVRTC-compressed texture: PVRTC2
        PVRTC2,
        //! 2-bit PVRTC-compressed texture: PVRTC2 (has alpha channel)
        PVRTC2A,
        //! ETC-compressed texture: ETC
        ETC,
        //! S3TC-compressed texture: S3TC_Dxt1
        S3TC_DXT1,
        //! S3TC-compressed texture: S3TC_Dxt3
        S3TC_DXT3,
        //! S3TC-compressed texture: S3TC_Dxt5
        S3TC_DXT5,
        //! ATITC-compressed texture: ATC_RGB
        ATC_RGB,
        //! ATITC-compressed texture: ATC_EXPLICIT_ALPHA
        ATC_EXPLICIT_ALPHA,
        //! ATITC-compresed texture: ATC_INTERPOLATED_ALPHA
        ATC_INTERPOLATED_ALPHA,
        //! Default texture format: AUTO
        DEFAULT = AUTO,
        
        NONE = -1
    };
    
    
    struct PixelFormatInfo {

        PixelFormatInfo(GLenum anInternalFormat, GLenum aFormat, GLenum aType, int aBpp, bool aCompressed, bool anAlpha)
            : internalFormat(anInternalFormat)
            , format(aFormat)
            , type(aType)
            , bpp(aBpp)
            , compressed(aCompressed)
            , alpha(anAlpha)
        {}

        GLenum internalFormat;
        GLenum format;
        GLenum type;
        int bpp;
        bool compressed;
        bool alpha;
    };
    
    typedef std::map<Texture2D::PixelFormat, const PixelFormatInfo> PixelFormatInfoMap;
    
    /**
     Extension to set the Min / Mag filter
     */
    typedef struct _TexParams {
        GLuint    minFilter;
        GLuint    magFilter;
        GLuint    wrapS;
        GLuint    wrapT;
    }TexParams;
    
public:
	static const PixelFormatInfoMap& getPixelFormatInfoMap();

public:

	/** Gets the pixel format of the texture */
	Texture2D::PixelFormat getPixelFormat() const;

	/** Gets the width of the texture in pixels */
	int getPixelsWide() const;

	/** Gets the height of the texture in pixels */
	int getPixelsHigh() const;

	Size getContentSizeInPixel();

	/** Gets the texture name */
	GLuint getName() const;

	bool initWithImage(Image * image);
	bool initWithImage(Image *image, PixelFormat format);
	bool initWithMipmaps(MipmapInfo* mipmaps, int mipmapsNum, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh);
	bool initWithData(const void *data, ssize_t dataLen, Texture2D::PixelFormat pixelFormat, int pixelsWide, int pixelsHigh, const Size& contentSize);

protected:
	static const PixelFormatInfoMap _pixelFormatInfoTables;
private:
	/**convert functions*/

	/**
	Convert the format to the format param you specified, if the format is PixelFormat::Automatic, it will detect it automatically and convert to the closest format for you.
	It will return the converted format to you. if the outData != data, you must delete it manually.
	*/
	static PixelFormat convertDataToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat originFormat, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);

	static PixelFormat convertI8ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
	static PixelFormat convertAI88ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
	static PixelFormat convertRGB888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);
	static PixelFormat convertRGBA8888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen);

	//I8 to XXX
	static void convertI8ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertI8ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

	//AI88 to XXX
	static void convertAI88ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertAI88ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

	//RGB888 to XXX
	static void convertRGB888ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGB888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

	//RGBA8888 to XXX
	static void convertRGBA8888ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData);
	static void convertRGBA8888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData);

protected:
	/** pixel format of the texture */
	Texture2D::PixelFormat _pixelFormat;

	/** width in pixels */
	int _pixelsWide;

	/** height in pixels */
	int _pixelsHigh;

	/** texture name */
	GLuint _name;

	Size _contentSize;

};




#endif //__CCTEXTURE2D_H__

