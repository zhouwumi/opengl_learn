#include "GLView.h"
#include "render/QuadCommand.h"
#include "Director.h"
#include "render/RenderTexture.h"
#include "ClippingNode.h"
#include "2d/DrawNode.h"


class GLFWEventHandler
{
public:
	static void onGLFWError(int errorID, const char* errorDesc)
	{
		if (_view)
			_view->onGLFWError(errorID, errorDesc);
	}

	static void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
	{
		if (_view)
			_view->onGLFWMouseCallBack(window, button, action, modify);
	}

	static void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
	{
		if (_view)
			_view->onGLFWMouseMoveCallBack(window, x, y);
	}

	static void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
	{
		if (_view)
			_view->onGLFWMouseScrollCallback(window, x, y);
	}

	static void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (_view)
			_view->onGLFWKeyCallback(window, key, scancode, action, mods);
	}

	static void onGLFWCharCallback(GLFWwindow* window, unsigned int character)
	{
		if (_view)
			_view->onGLFWCharCallback(window, character);
	}

	static void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y)
	{
		if (_view)
			_view->onGLFWWindowPosCallback(windows, x, y);
	}

	static void onGLFWframebuffersize(GLFWwindow* window, int w, int h)
	{
		if (_view)
			_view->onGLFWframebuffersize(window, w, h);
	}

	static void onGLFWWindowSizeFunCallback(GLFWwindow *window, int width, int height)
	{
		if (_view)
			_view->onGLFWWindowSizeFunCallback(window, width, height);
	}

	static void setGLViewImpl(GLView* view)
	{
		_view = view;
	}

	static void onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified)
	{
		if (_view)
		{
			_view->onGLFWWindowIconifyCallback(window, iconified);
		}
	}

private:
	static GLView* _view;
};


GLView* GLFWEventHandler::_view = nullptr;

GLView::GLView()
{
	GLFWEventHandler::setGLViewImpl(this);
}

void GLView::onGLFWCharCallback(GLFWwindow* window, unsigned int character)
{

}

void GLView::onGLFWError(int errorID, const char* errorDesc)
{

}

void GLView::onGLFWframebuffersize(GLFWwindow* window, int w, int h)
{

}

void GLView::onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void GLView::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
{

}

void GLView::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{

}

void GLView::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
{

}

void GLView::onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified)
{

}

void GLView::onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y)
{

}

void GLView::onGLFWWindowSizeFunCallback(GLFWwindow *window, int width, int height)
{

}

void GLView::setViewContentAtts(GLViewContentAtts glContextAttrs)
{

}

bool GLView::createWindow(std::string windowname)
{

	if (!glfwInit())
	{
		return false;
	}
	_glContextAttrs = { 8, 8, 8, 8, 24,8 };

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_RED_BITS, _glContextAttrs.redBits);
	glfwWindowHint(GLFW_GREEN_BITS, _glContextAttrs.greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, _glContextAttrs.blueBits);
	glfwWindowHint(GLFW_ALPHA_BITS, _glContextAttrs.alphaBits);
	glfwWindowHint(GLFW_DEPTH_BITS, _glContextAttrs.depthBits);
	glfwWindowHint(GLFW_STENCIL_BITS, _glContextAttrs.stencilBits);
	
	Size winSize = Director::getInstance()->getWinSize();
	_mainWindow = glfwCreateWindow(winSize.width, winSize.height, windowname.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(_mainWindow);  //在我们使用 GL 命令之前我们需要将创建的窗口设置为当前窗口

	//glViewport(0, 0, 480, 320);
	
	glfwSetMouseButtonCallback(_mainWindow, GLFWEventHandler::onGLFWMouseCallBack);
	glfwSetCursorPosCallback(_mainWindow, GLFWEventHandler::onGLFWMouseMoveCallBack);
	glfwSetScrollCallback(_mainWindow, GLFWEventHandler::onGLFWMouseScrollCallback);
	glfwSetCharCallback(_mainWindow, GLFWEventHandler::onGLFWCharCallback);
	glfwSetKeyCallback(_mainWindow, GLFWEventHandler::onGLFWKeyCallback);
	glfwSetWindowPosCallback(_mainWindow, GLFWEventHandler::onGLFWWindowPosCallback);
	glfwSetFramebufferSizeCallback(_mainWindow, GLFWEventHandler::onGLFWframebuffersize);
	glfwSetWindowSizeCallback(_mainWindow, GLFWEventHandler::onGLFWWindowSizeFunCallback);
	glfwSetWindowIconifyCallback(_mainWindow, GLFWEventHandler::onGLFWWindowIconifyCallback);
	
	initGlew();
	return true;
}



void GLView::initShaderProgram()
{
	//this->m_glProgram.initWithSource(ccPositionTextureColor_noMVP_vert, ccPositionTextureColor_noMVP_frag);
}


void GLView::initTexture2d()
{

	Sprite* sprite1 = Sprite::createWithFileName("E://2.jpg");
	_sprites.push_back(sprite1);
	sprite1->setName("sprite1");
	sprite1->setPosition(0, 0);
	sprite1->setScale(2);

//	Sprite* sprite2 = Sprite::createWithFileName("E://3.png");
//	_sprites.push_back(sprite2);
//	sprite2->setPosition(250, 200);

}


void GLView::render()
{
	m_render = Director::getInstance()->getRenderer();
	initTexture2d();

	ClippingNode* clippingNode = ClippingNode::create();
	Sprite* stencil = Sprite::createWithFileName("E://5.png");
	Sprite* back = Sprite::createWithFileName("E://3.png");
//	back->setPosition(200, 10);
	stencil->setPosition(200, 10);
	clippingNode->setSprite(back);
	clippingNode->setStencil(stencil);
	clippingNode->setAlpha(1.0);

	bool saveImage = false;  //截图功能 RenderTexure
	bool testClippingNode = false;  //遮罩功能ClippingNode
	bool testDrawNodePoint = false;  //DrawNode功能之画点
	bool testDrawNodeLine = true; //DrawNode功能之画线

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(_mainWindow))
	{
		glClearColor(0.2, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		

		if (saveImage)
		{
			/*  测试 RenderTexture
			Size size = Director::getInstance()->getWinSize();
			RenderTexture* renderTexure = RenderTexture::create(20,20);
			renderTexure->begin();
			renderTexure->end();

			renderTexure->onBegin();
			for (Sprite* sprite : _sprites)
			{
				sprite->setPosition(20, 20);
				sprite->visit(&m_render);
				sprite->setPosition(0, 0);
			}
			m_render.draw();
			renderTexure->onEnd();
			renderTexure->saveToFile("1.png");
			saveImage = false;

			CC_SAFE_DELETE(renderTexure);*/
		}
		else if (testClippingNode){
			/* 测试ClippingNode
			auto _director = Director::getInstance();
			clippingNode->onBeforeVisit();
			
			stencil->visit(&m_render);
			m_render.draw();
			clippingNode->onAfterVisitStencil();

			back->visit(&m_render);
			m_render.draw();

			clippingNode->onAfterVisit();*/
		} else if (testDrawNodePoint)
		{
			DrawNode* drawNode = DrawNode::create();
			CHECK_GL_ERROR_DEBUG();
			for (int i = 1; i <= 100; i++)
			{
				drawNode->drawPoint(Vec2(100+i, 100), 1, Color4F(255.0, 0, 0, 255.0));
			}
			
			drawNode->onDrawPoinit();
	//		CHECK_GL_ERROR_DEBUG();
		}else if (testDrawNodeLine)
		{
			DrawNode* drawNode = DrawNode::create();
			CHECK_GL_ERROR_DEBUG();
			drawNode->drawLine(Vec2(100, 100), Vec2(200, 200), Color4F(255.0, 0, 0, 255.0));
			drawNode->drawLine(Vec2(100, 200), Vec2(200, 100), Color4F(255.0, 0, 0, 255.0));
			drawNode->onDrawLine();
		}
		else{
			
			for (Sprite* sprite : _sprites)
			{
				sprite->visit(&m_render);
			}

			m_render.draw();

		}
		

		/* Swap front and back buffers */
		glfwSwapBuffers(_mainWindow);

		/* Poll for and process events */
		glfwPollEvents();

	}
}

bool GLView::initGlew()
{
	GLenum ret = glewInit();
	if (ret != GLEW_OK)
	{
		return false;
	}
	return true;
}

void GLView::setDesignResolution(size_t width, size_t height)
{
	_designResolutionSize.setSize(width, height);
}

Size GLView::getDesignResolution()
{
	return _designResolutionSize;
}

GLView::~GLView()
{
	glfwTerminate();
	for (Sprite* sprite : _sprites)
	{
		CC_SAFE_DELETE(sprite);
	}
}