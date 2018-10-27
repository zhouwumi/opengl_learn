#ifndef  _Common_Util_H
#define _Common_Util_H

#include <algorithm>

/// The max length of CCLog message.
static const int MAX_LOG_LENGTH = 16 * 1024;

namespace util{
	void log(const char *format, ...);
};

#endif