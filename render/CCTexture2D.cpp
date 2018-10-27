#include "render/CCTexture2D.h"
#include "glew.h"
#include "CCGeometry.h"
#include "CCImage.h"
#include "ccTypes.h"


typedef Texture2D::PixelFormatInfoMap::value_type PixelFormatInfoMapValue;
static const PixelFormatInfoMapValue TexturePixelFormatInfoTablesValue[] =
{
	PixelFormatInfoMapValue(Texture2D::PixelFormat::BGRA8888, Texture2D::PixelFormatInfo(GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE, 32, false, true)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::RGBA8888, Texture2D::PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 32, false, true)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::RGBA4444, Texture2D::PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 16, false, true)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::RGB5A1, Texture2D::PixelFormatInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, 16, false, true)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::RGB565, Texture2D::PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 16, false, false)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::RGB888, Texture2D::PixelFormatInfo(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, 24, false, false)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::A8, Texture2D::PixelFormatInfo(GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE, 8, false, false)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::I8, Texture2D::PixelFormatInfo(GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, 8, false, false)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::AI88, Texture2D::PixelFormatInfo(GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, 16, false, true)),

#ifdef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
	PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC2, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true, false)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC2A, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 2, true, true)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC4, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
	PixelFormatInfoMapValue(Texture2D::PixelFormat::PVRTC4A, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, true)),
#endif

#ifdef GL_ETC1_RGB8_OES
	PixelFormatInfoMapValue(Texture2D::PixelFormat::ETC, Texture2D::PixelFormatInfo(GL_ETC1_RGB8_OES, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
	PixelFormatInfoMapValue(Texture2D::PixelFormat::S3TC_DXT1, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
	PixelFormatInfoMapValue(Texture2D::PixelFormat::S3TC_DXT3, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	PixelFormatInfoMapValue(Texture2D::PixelFormat::S3TC_DXT5, Texture2D::PixelFormatInfo(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_ATC_RGB_AMD
	PixelFormatInfoMapValue(Texture2D::PixelFormat::ATC_RGB, Texture2D::PixelFormatInfo(GL_ATC_RGB_AMD,
	0xFFFFFFFF, 0xFFFFFFFF, 4, true, false)),
#endif

#ifdef GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
	PixelFormatInfoMapValue(Texture2D::PixelFormat::ATC_EXPLICIT_ALPHA, Texture2D::PixelFormatInfo(GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,
	0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif

#ifdef GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
	PixelFormatInfoMapValue(Texture2D::PixelFormat::ATC_INTERPOLATED_ALPHA, Texture2D::PixelFormatInfo(GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,
	0xFFFFFFFF, 0xFFFFFFFF, 8, true, false)),
#endif
};

static Texture2D::PixelFormat g_defaultAlphaPixelFormat = Texture2D::PixelFormat::DEFAULT;

const Texture2D::PixelFormatInfoMap Texture2D::_pixelFormatInfoTables(TexturePixelFormatInfoTablesValue,
	TexturePixelFormatInfoTablesValue + sizeof(TexturePixelFormatInfoTablesValue) / sizeof(TexturePixelFormatInfoTablesValue[0]));


const Texture2D::PixelFormatInfoMap& Texture2D::getPixelFormatInfoMap()
{
	return _pixelFormatInfoTables;
}

// IIIIIIII -> RRRRRRRRGGGGGGGGGBBBBBBBB
void Texture2D::convertI8ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0; i < dataLen; ++i)
	{
		*outData++ = data[i];     //R
		*outData++ = data[i];     //G
		*outData++ = data[i];     //B
	}
}

// IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
void Texture2D::convertAI88ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
	{
		*outData++ = data[i];     //R
		*outData++ = data[i];     //G
		*outData++ = data[i];     //B
	}
}

// IIIIIIII -> RRRRRRRRGGGGGGGGGBBBBBBBBAAAAAAAA
void Texture2D::convertI8ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0; i < dataLen; ++i)
	{
		*outData++ = data[i];     //R
		*outData++ = data[i];     //G
		*outData++ = data[i];     //B
		*outData++ = 0xFF;        //A
	}
}

// IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
void Texture2D::convertAI88ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
	{
		*outData++ = data[i];     //R
		*outData++ = data[i];     //G
		*outData++ = data[i];     //B
		*outData++ = data[i + 1]; //A
	}
}

// IIIIIIII -> RRRRRGGGGGGBBBBB
void Texture2D::convertI8ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (int i = 0; i < dataLen; ++i)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i] & 0x00FC) << 3         //G
			| (data[i] & 0x00F8) >> 3;        //B
	}
}

// IIIIIIIIAAAAAAAA -> RRRRRGGGGGGBBBBB
void Texture2D::convertAI88ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i] & 0x00FC) << 3         //G
			| (data[i] & 0x00F8) >> 3;        //B
	}
}

// IIIIIIII -> RRRRGGGGBBBBAAAA
void Texture2D::convertI8ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0; i < dataLen; ++i)
	{
		*out16++ = (data[i] & 0x00F0) << 8    //R
			| (data[i] & 0x00F0) << 4             //G
			| (data[i] & 0x00F0)                  //B
			| 0x000F;                             //A
	}
}

// IIIIIIIIAAAAAAAA -> RRRRGGGGBBBBAAAA
void Texture2D::convertAI88ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
	{
		*out16++ = (data[i] & 0x00F0) << 8    //R
			| (data[i] & 0x00F0) << 4             //G
			| (data[i] & 0x00F0)                  //B
			| (data[i + 1] & 0x00F0) >> 4;          //A
	}
}

// IIIIIIII -> RRRRRGGGGGBBBBBA
void Texture2D::convertI8ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (int i = 0; i < dataLen; ++i)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i] & 0x00F8) << 3         //G
			| (data[i] & 0x00F8) >> 2         //B
			| 0x0001;                         //A
	}
}

// IIIIIIIIAAAAAAAA -> RRRRRGGGGGBBBBBA
void Texture2D::convertAI88ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i] & 0x00F8) << 3         //G
			| (data[i] & 0x00F8) >> 2         //B
			| (data[i + 1] & 0x0080) >> 7;    //A
	}
}

// IIIIIIII -> IIIIIIIIAAAAAAAA
void Texture2D::convertI8ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0; i < dataLen; ++i)
	{
		*out16++ = 0xFF00     //A
			| data[i];            //I
	}
}

// IIIIIIIIAAAAAAAA -> AAAAAAAA
void Texture2D::convertAI88ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 1; i < dataLen; i += 2)
	{
		*outData++ = data[i]; //A
	}
}

// IIIIIIIIAAAAAAAA -> IIIIIIII
void Texture2D::convertAI88ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
	{
		*outData++ = data[i]; //R
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
void Texture2D::convertRGB888ToRGBA8888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
	{
		*outData++ = data[i];         //R
		*outData++ = data[i + 1];     //G
		*outData++ = data[i + 2];     //B
		*outData++ = 0xFF;            //A
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
void Texture2D::convertRGBA8888ToRGB888(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
	{
		*outData++ = data[i];         //R
		*outData++ = data[i + 1];     //G
		*outData++ = data[i + 2];     //B
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGGBBBBB
void Texture2D::convertRGB888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i + 1] & 0x00FC) << 3     //G
			| (data[i + 2] & 0x00F8) >> 3;    //B
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRGGGGGGBBBBB
void Texture2D::convertRGBA8888ToRGB565(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i + 1] & 0x00FC) << 3     //G
			| (data[i + 2] & 0x00F8) >> 3;    //B
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIII
void Texture2D::convertRGB888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
	{
		*outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIII
void Texture2D::convertRGBA8888ToI8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
	{
		*outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> AAAAAAAA
void Texture2D::convertRGBA8888ToA8(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
	{
		*outData++ = data[i + 3]; //A
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIIIAAAAAAAA
void Texture2D::convertRGB888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
	{
		*outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
		*outData++ = 0xFF;
	}
}


// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIIIAAAAAAAA
void Texture2D::convertRGBA8888ToAI88(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
	{
		*outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
		*outData++ = data[i + 3];
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRGGGGBBBBAAAA
void Texture2D::convertRGB888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
	{
		*out16++ = ((data[i] & 0x00F0) << 8           //R
			| (data[i + 1] & 0x00F0) << 4     //G
			| (data[i + 2] & 0xF0)            //B
			| 0x0F);                         //A
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRGGGGBBBBAAAA
void Texture2D::convertRGBA8888ToRGBA4444(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
	{
		*out16++ = (data[i] & 0x00F0) << 8    //R
			| (data[i + 1] & 0x00F0) << 4         //G
			| (data[i + 2] & 0xF0)                //B
			| (data[i + 3] & 0xF0) >> 4;         //A
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGBBBBBA
void Texture2D::convertRGB888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i + 1] & 0x00F8) << 3     //G
			| (data[i + 2] & 0x00F8) >> 2     //B
			| 0x01;                          //A
	}
}

// RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGBBBBBA
void Texture2D::convertRGBA8888ToRGB5A1(const unsigned char* data, ssize_t dataLen, unsigned char* outData)
{
	unsigned short* out16 = (unsigned short*)outData;
	for (ssize_t i = 0, l = dataLen - 2; i < l; i += 4)
	{
		*out16++ = (data[i] & 0x00F8) << 8    //R
			| (data[i + 1] & 0x00F8) << 3     //G
			| (data[i + 2] & 0x00F8) >> 2     //B
			| (data[i + 3] & 0x0080) >> 7;   //A
	}
}


Texture2D::PixelFormat Texture2D::convertDataToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat originFormat, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
	// don't need to convert
	if (format == originFormat || format == PixelFormat::AUTO)
	{
		*outData = (unsigned char*)data;
		*outDataLen = dataLen;
		return originFormat;
	}

	switch (originFormat)
	{
	case PixelFormat::I8:
		return convertI8ToFormat(data, dataLen, format, outData, outDataLen);
	case PixelFormat::AI88:
		return convertAI88ToFormat(data, dataLen, format, outData, outDataLen);
	case PixelFormat::RGB888:
		return convertRGB888ToFormat(data, dataLen, format, outData, outDataLen);
	case PixelFormat::RGBA8888:
		return convertRGBA8888ToFormat(data, dataLen, format, outData, outDataLen);
	default:
		CCLOG("unsupport convert for format %d to format %d", originFormat, format);
		*outData = (unsigned char*)data;
		*outDataLen = dataLen;
		return originFormat;
	}
}


Texture2D::PixelFormat Texture2D::convertI8ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
	switch (format)
	{
	case PixelFormat::RGBA8888:
		*outDataLen = dataLen * 4;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertI8ToRGBA8888(data, dataLen, *outData);
		break;
	case PixelFormat::RGB888:
		*outDataLen = dataLen * 3;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertI8ToRGB888(data, dataLen, *outData);
		break;
	case PixelFormat::RGB565:
		*outDataLen = dataLen * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertI8ToRGB565(data, dataLen, *outData);
		break;
	case PixelFormat::AI88:
		*outDataLen = dataLen * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertI8ToAI88(data, dataLen, *outData);
		break;
	case PixelFormat::RGBA4444:
		*outDataLen = dataLen * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertI8ToRGBA4444(data, dataLen, *outData);
		break;
	case PixelFormat::RGB5A1:
		*outDataLen = dataLen * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertI8ToRGB5A1(data, dataLen, *outData);
		break;
	default:
		// unsupport convertion or don't need to convert
		if (format != PixelFormat::AUTO && format != PixelFormat::I8)
		{
			CCLOG("Can not convert image format PixelFormat::I8 to format ID:%d, we will use it's origin format PixelFormat::I8", format);
		}

		*outData = (unsigned char*)data;
		*outDataLen = dataLen;
		return PixelFormat::I8;
	}

	return format;
}

Texture2D::PixelFormat Texture2D::convertAI88ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
	switch (format)
	{
	case PixelFormat::RGBA8888:
		*outDataLen = dataLen * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertAI88ToRGBA8888(data, dataLen, *outData);
		break;
	case PixelFormat::RGB888:
		*outDataLen = dataLen / 2 * 3;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertAI88ToRGB888(data, dataLen, *outData);
		break;
	case PixelFormat::RGB565:
		*outDataLen = dataLen;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertAI88ToRGB565(data, dataLen, *outData);
		break;
	case PixelFormat::A8:
		*outDataLen = dataLen / 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertAI88ToA8(data, dataLen, *outData);
		break;
	case PixelFormat::I8:
		*outDataLen = dataLen / 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertAI88ToI8(data, dataLen, *outData);
		break;
	case PixelFormat::RGBA4444:
		*outDataLen = dataLen;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertAI88ToRGBA4444(data, dataLen, *outData);
		break;
	case PixelFormat::RGB5A1:
		*outDataLen = dataLen;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertAI88ToRGB5A1(data, dataLen, *outData);
		break;
	default:
		// unsupport convertion or don't need to convert
		if (format != PixelFormat::AUTO && format != PixelFormat::AI88)
		{
			CCLOG("Can not convert image format PixelFormat::AI88 to format ID:%d, we will use it's origin format PixelFormat::AI88", format);
		}

		*outData = (unsigned char*)data;
		*outDataLen = dataLen;
		return PixelFormat::AI88;
		break;
	}

	return format;
}

Texture2D::PixelFormat Texture2D::convertRGB888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{
	switch (format)
	{
	case PixelFormat::RGBA8888:
		*outDataLen = dataLen / 3 * 4;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGB888ToRGBA8888(data, dataLen, *outData);
		break;
	case PixelFormat::RGB565:
		*outDataLen = dataLen / 3 * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGB888ToRGB565(data, dataLen, *outData);
		break;
	case PixelFormat::I8:
		*outDataLen = dataLen / 3;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGB888ToI8(data, dataLen, *outData);
		break;
	case PixelFormat::AI88:
		*outDataLen = dataLen / 3 * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGB888ToAI88(data, dataLen, *outData);
		break;
	case PixelFormat::RGBA4444:
		*outDataLen = dataLen / 3 * 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGB888ToRGBA4444(data, dataLen, *outData);
		break;
	case PixelFormat::RGB5A1:
		*outDataLen = dataLen;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGB888ToRGB5A1(data, dataLen, *outData);
		break;
	default:
		// unsupport convertion or don't need to convert
		if (format != PixelFormat::AUTO && format != PixelFormat::RGB888)
		{
			CCLOG("Can not convert image format PixelFormat::RGB888 to format ID:%d, we will use it's origin format PixelFormat::RGB888", format);
		}

		*outData = (unsigned char*)data;
		*outDataLen = dataLen;
		return PixelFormat::RGB888;
	}
	return format;
}

Texture2D::PixelFormat Texture2D::convertRGBA8888ToFormat(const unsigned char* data, ssize_t dataLen, PixelFormat format, unsigned char** outData, ssize_t* outDataLen)
{

	switch (format)
	{
	case PixelFormat::RGB888:
		*outDataLen = dataLen / 4 * 3;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGBA8888ToRGB888(data, dataLen, *outData);
		break;
	case PixelFormat::RGB565:
		*outDataLen = dataLen / 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGBA8888ToRGB565(data, dataLen, *outData);
		break;
	case PixelFormat::A8:
		*outDataLen = dataLen / 4;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGBA8888ToA8(data, dataLen, *outData);
		break;
	case PixelFormat::I8:
		*outDataLen = dataLen / 4;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGBA8888ToI8(data, dataLen, *outData);
		break;
	case PixelFormat::AI88:
		*outDataLen = dataLen / 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGBA8888ToAI88(data, dataLen, *outData);
		break;
	case PixelFormat::RGBA4444:
		*outDataLen = dataLen / 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGBA8888ToRGBA4444(data, dataLen, *outData);
		break;
	case PixelFormat::RGB5A1:
		*outDataLen = dataLen / 2;
		*outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
		convertRGBA8888ToRGB5A1(data, dataLen, *outData);
		break;
	default:
		// unsupport convertion or don't need to convert
		if (format != PixelFormat::AUTO && format != PixelFormat::RGBA8888)
		{
			CCLOG("Can not convert image format PixelFormat::RGBA8888 to format ID:%d, we will use it's origin format PixelFormat::RGBA8888", format);
		}

		*outData = (unsigned char*)data;
		*outDataLen = dataLen;
		return PixelFormat::RGBA8888;
	}

	return format;
}



Texture2D::PixelFormat Texture2D::getPixelFormat() const
{
	return _pixelFormat;
}

int Texture2D::getPixelsWide() const
{
	return _pixelsWide;
}

int Texture2D::getPixelsHigh() const
{
	return _pixelsHigh;
}

GLuint Texture2D::getName() const
{
	return _name;
}

// implementation Texture2D (Image)
bool Texture2D::initWithImage(Image *image)
{
	return initWithImage(image, g_defaultAlphaPixelFormat);
}

bool Texture2D::initWithImage(Image *image, PixelFormat format)
{
	if (image == nullptr)
	{
		CCLOG("cocos2d: Texture2D. Can't create Texture. UIImage is nil");
		return false;
	}

	int imageWidth = image->getWidth();
	int imageHeight = image->getHeight();


	unsigned char*   tempData = image->getData();
	Size             imageSize = Size((float)imageWidth, (float)imageHeight);
	PixelFormat      pixelFormat = ((PixelFormat::NONE == format) || (PixelFormat::AUTO == format)) ? image->getRenderFormat() : format;
	PixelFormat      renderFormat = image->getRenderFormat();
	size_t	         tempDataLen = image->getDataLen();


	if (image->getNumberOfMipmaps() > 1)
	{
		if (pixelFormat != image->getRenderFormat())
		{
			CCLOG("cocos2d: WARNING: This image has more than 1 mipmaps and we will not convert the data format");
		}

		initWithMipmaps(image->getMipmaps(), image->getNumberOfMipmaps(), image->getRenderFormat(), imageWidth, imageHeight);

		return true;
	}
	else if (image->isCompressed())
	{
		if (pixelFormat != image->getRenderFormat())
		{
			CCLOG("cocos2d: WARNING: This image is compressed and we cann't convert it for now");
		}

		initWithData(tempData, tempDataLen, image->getRenderFormat(), imageWidth, imageHeight, imageSize);
		return true;
	}
	else
	{
		unsigned char* outTempData = nullptr;
		ssize_t outTempDataLen = 0;

		pixelFormat = convertDataToFormat(tempData, tempDataLen, renderFormat, pixelFormat, &outTempData, &outTempDataLen);

		initWithData(outTempData, outTempDataLen, pixelFormat, imageWidth, imageHeight, imageSize);


		if (outTempData != nullptr && outTempData != tempData)
		{

			free(outTempData);
		}

		return true;
	}
}

bool Texture2D::initWithData(const void *data, ssize_t dataLen, Texture2D::PixelFormat pixelFormat, int pixelsWide, int pixelsHigh, const Size& contentSize)
{
	CCASSERT(dataLen > 0 && pixelsWide > 0 && pixelsHigh > 0, "Invalid size");

	//if data has no mipmaps, we will consider it has only one mipmap
	MipmapInfo mipmap;
	mipmap.address = (unsigned char*)data;
	mipmap.len = static_cast<int>(dataLen);
	return initWithMipmaps(&mipmap, 1, pixelFormat, pixelsWide, pixelsHigh);
}

bool Texture2D::initWithMipmaps(MipmapInfo* mipmaps, int mipmapsNum, PixelFormat pixelFormat, int pixelsWide, int pixelsHigh)
{

	//the pixelFormat must be a certain value 
	CCASSERT((pixelFormat != PixelFormat::NONE && pixelFormat != PixelFormat::AUTO), "the \"pixelFormat\" param must be a certain value!");
	CCASSERT(pixelsWide > 0 && pixelsHigh > 0, "Invalid size");

	if (mipmapsNum <= 0)
	{
		CCLOG("cocos2d: WARNING: mipmap number is less than 1");
		return false;
	}


	if (_pixelFormatInfoTables.find(pixelFormat) == _pixelFormatInfoTables.end())
	{
		CCLOG("cocos2d: WARNING: unsupported pixelformat: %lx", (unsigned long)pixelFormat);
		return false;
	}

	const PixelFormatInfo& info = _pixelFormatInfoTables.at(pixelFormat);

	//Set the row align only when mipmapsNum == 1 and the data is uncompressed

	if (mipmapsNum == 1 && !info.compressed)
	{
		unsigned int bytesPerRow = pixelsWide * info.bpp / 8;


		if (bytesPerRow % 8 == 0)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
		}
		else if (bytesPerRow % 4 == 0)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		else if (bytesPerRow % 2 == 0)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		}
		else
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
	}
	else
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	if (_name != 0)
	{
		glDeleteTextures(1, &_name);
		_name = 0;
	}

	glGenTextures(1, &_name);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, _name);

	if (mipmapsNum == 1)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//纹理缩小用线性过滤
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理放大用线性过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



#if CC_ENABLE_CACHE_TEXTURE_DATA
	if (_antialiasEnabled)
	{
		TexParams texParams = { (GLuint)(_hasMipmaps ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR), GL_LINEAR, GL_NONE, GL_NONE };
		VolatileTextureMgr::setTexParameters(this, texParams);
	}
	else
	{
		TexParams texParams = { (GLuint)(_hasMipmaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST), GL_NEAREST, GL_NONE, GL_NONE };
		VolatileTextureMgr::setTexParameters(this, texParams);
	}
#endif


	// Specify OpenGL texture image
	int width = pixelsWide;
	int height = pixelsHigh;

	for (int i = 0; i < mipmapsNum; ++i)
	{
		unsigned char *data = mipmaps[i].address;
		GLsizei datalen = mipmaps[i].len;

		if (info.compressed)
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, i, info.internalFormat, (GLsizei)width, (GLsizei)height, 0, datalen, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, i, info.internalFormat, (GLsizei)width, (GLsizei)height, 0, info.format, info.type, data);
		}

		if (i > 0 && (width != height))
		{
			CCLOG("cocos2d: Texture2D. WARNING. Mipmap level %u is not squared. Texture won't render correctly. width=%d != height=%d", i, width, height);
		}

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			CCLOG("cocos2d: Texture2D: Error uploading compressed texture level: %u . glError: 0x%04X", i, err);
			return false;
		}

		width = MAX(width >> 1, 1);
		height = MAX(height >> 1, 1);
	}


	_pixelsWide = pixelsWide;
	_pixelsHigh = pixelsHigh;
	_pixelFormat = pixelFormat;
	_contentSize = Size((float)pixelsWide, (float)pixelsHigh);
	return true;
}

Size Texture2D::getContentSizeInPixel()
{
	return _contentSize;
}