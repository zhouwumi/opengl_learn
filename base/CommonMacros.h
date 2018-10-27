#ifndef  _ENCRYPE_TOOL_COMMONMACROS_H
#define  _ENCRYPE_TOOL_COMMONMACROS_H

#include <stdio.h>
#include <assert.h>
#define NOMINMAX
#include <windows.h>
#include "CommonUtil.h"

typedef LONG_PTR SSIZE_T;

#ifndef __SSIZE_T
#define __SSIZE_T
typedef SSIZE_T ssize_t;
#endif // __SSIZE_T

#define STRINGIFY(A)  #A

#define CC_DLL     __declspec(dllexport)
#define CC_UNUSED_PARAM(unusedparam) (void)unusedparam

#define  CC_BREAK_IF(cloud) if (cloud){ break;}

#define CC_ASSERT(cloud) assert(cloud)

#define CHAR_SIZE  sizeof(char)

#define  CCASSERT(cloud,msg) do{				\
		if(!cloud){				\
			if(msg && strlen(msg)>0){		\
				printf("%s\n",msg);		\
															}		\
			assert(cloud)	;		\
								}		\
	} while(0)

#define  CCLOG(msg) printf("%s\n",msg)

#define  CCLOGMSG(format,...) util::log(format,##__VA_ARGS__)

#define CCLOGERROR(format,...)  util::log(format,##__VA_ARGS__)

#define CC_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100) 
#define CC_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define CC_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)   
#define CC_SWAP_INT32_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP32(i) : (i) )
#define CC_SWAP_INT16_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP16(i) : (i) )
#define CC_SWAP_INT32_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i) : CC_SWAP32(i) )
#define CC_SWAP_INT16_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i):  CC_SWAP16(i) )

typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;

#define CC_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define CC_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define CC_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define CC_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define CC_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define CC_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)


#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

#define GP_ASSERT(cloud) CCASSERT(cloud,"")

/** @def CC_DEGREES_TO_RADIANS
converts degrees to radians
*/
#define CC_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f) // PI / 180

/** @def CC_RADIANS_TO_DEGREES
converts radians to degrees
*/
#define CC_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f) // PI * 180

#define CHECK_GL_ERROR_DEBUG() \
    do { \
        GLenum __error = glGetError(); \
        if(__error) { \
            CCLOGMSG("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
				        } \
		    } while (false)

#endif

