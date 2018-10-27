#include <stdlib.h>
#include "Director.h"
#include "GLView.h"

int main()
{
	
	Director::getInstance()->setWinSize(Size(960, 640));
	Director::getInstance()->setProjection();
	GLView* glView = new GLView();
	if (glView->createWindow("OpenGL Lesson One"))
	{
		Director::getInstance()->initRender();
		glView->render();
	}

	delete glView;
	glView = nullptr; 


	system("pause");
	return 1;
}