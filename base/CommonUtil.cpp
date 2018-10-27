#include "CommonUtil.h"
#include <stdarg.h>

namespace util{

	void _log(const char *format, va_list args)
	{
		char buf[MAX_LOG_LENGTH];

		vsnprintf(buf, MAX_LOG_LENGTH - 3, format, args);
		strcat(buf, "\n");
		printf(buf);
	}

	void log(const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		_log(format, args);
		va_end(args);
	}
}
