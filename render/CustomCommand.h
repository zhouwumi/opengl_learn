#ifndef  _CUSTOM_COMAMND_H
#define _CUSTOM_COMAMND_H

#include <functional>

class CustomCommand{
public:
	std::function<void()> func;

	void init(std::function<void()> f);

	void execute();
};

#endif