#include "Director.h"

Director* Director::_defaultDirector = nullptr;

Director*  Director::getInstance()
{
	if (_defaultDirector == nullptr)
	{
		_defaultDirector = new (std::nothrow) Director();
		if (!_defaultDirector->init())
		{
			CC_SAFE_DELETE(_defaultDirector);
			return nullptr;
		}
	}
	return _defaultDirector;
}




void Director::initRender()
{
	render = new (std::nothrow) Render();
	render->init();
}

bool Director::init()
{
	initStack();
	return true;
}

void Director::initStack()
{
	_modalViewMatrixStack.push(Mat4::IDENTITY);
	_projectionMatrixStack.push(Mat4::IDENTITY);
}

void Director::setProjection()
{
	Mat4 matrixP, matrixPerspective, matrixLookup;

	Size size = Director::getInstance()->getWinSize();
	float zeye = (size.height / 1.1566f);

	Mat4::createPerspective(60, (GLfloat)size.width / size.height, 10, zeye + size.height / 2, &matrixPerspective);
	multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, matrixPerspective);
	Vec3 eye(size.width / 2, size.height / 2, zeye), center(size.width / 2, size.height / 2, 0.0f), up(0.0f, 1.0f, 0.0f);
	Mat4::createLookAt(eye, center, up, &matrixLookup);
	multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, matrixLookup);

}

GLView* Director::getGLView()
{
	return _glView;
}

void Director::drawScene()
{

}

Size Director::getWinSize()
{
	return _size;
}


void Director::setWinSize(Size& winSize)
{
	_size = winSize;
}

const Mat4& Director::getMatrix(MATRIX_STACK_TYPE type)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		return _modalViewMatrixStack.top();
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION){
		return _projectionMatrixStack.top();
	}

	CCLOG("invalide type");
	return nullptr;
}

void Director::loadIdentityMatrix(MATRIX_STACK_TYPE type)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		_modalViewMatrixStack.top() = Mat4::IDENTITY;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		_projectionMatrixStack.top() = Mat4::IDENTITY;
	}
	CCLOG("invalide type");

}

void Director::multiplyMatrix(MATRIX_STACK_TYPE type, Mat4& mat4)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		_modalViewMatrixStack.top() *= mat4;
		return;
	}else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		_projectionMatrixStack.top() *= mat4;
		return;
	}

	CCLOG("invalid type");
}

void Director::popMatrix(MATRIX_STACK_TYPE type)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		_modalViewMatrixStack.pop();
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		_projectionMatrixStack.pop();
	}

	CCLOG("invalid type");
}

void Director::loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat4)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		_modalViewMatrixStack.top() = mat4;
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		_projectionMatrixStack.top() = mat4;
	}
	CCLOG("invalid type");
}

void Director::pushMatrix(MATRIX_STACK_TYPE type)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		_modalViewMatrixStack.push(_modalViewMatrixStack.top());
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		_projectionMatrixStack.push(_projectionMatrixStack.top());
	}
	CCLOG("invalid type");
}