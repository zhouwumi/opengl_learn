#include "CustomCommand.h"

void CustomCommand::init(std::function<void()> f)
{
	func = f;
}

void CustomCommand::execute()
{
	if (func){
		func();
	}
}