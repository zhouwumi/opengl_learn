#include "RenderTexture.h"
#include "Director.h"
#include "render/Render.h"



RenderTexture* RenderTexture::create(int width, int height)
{
	RenderTexture* ret = new (std::nothrow) RenderTexture();

	if (!ret->init(width, height))
	{
		delete ret;
		ret = nullptr;
		return nullptr;
	}
	return ret;
}

RenderTexture::RenderTexture():
_keepMatrix(true)
{

}

bool RenderTexture::init(int width, int height)
{

	bool ret = false;
	void *data = nullptr;
	do
	{
		//Size size = Director::getInstance()->getWinSizeInPixels();
		//_fullviewPort = Rect(0,0,size.width,size.height);

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);

		// textures must be power of two squared
		int powW = width;
		int powH = height;


		auto dataLen = powW * powH * 4;
		data = malloc(dataLen);
		CC_BREAK_IF(!data);

		memset(data, 0, dataLen);
		_pixelFormat = Texture2D::PixelFormat::RGBA8888;

		_texture = new (std::nothrow) Texture2D();
		if (_texture)
		{
			_texture->initWithData(data, dataLen, (Texture2D::PixelFormat)_pixelFormat, powW, powH, Size((float)width, (float)height));
		}
		else
		{
			break;
		}
		GLint oldRBO;
		glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);

		// generate FBO
		glGenFramebuffers(1, &_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

		// associate texture with FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->getName(), 0);

		/*
		GLuint depthStencilFormat = GL_DEPTH24_STENCIL8;
		if (depthStencilFormat != 0)
		{
			//create and attach depth buffer
			glGenRenderbuffers(1, &_depthRenderBufffer);
			glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBufffer);
			glRenderbufferStorage(GL_RENDERBUFFER, depthStencilFormat, (GLsizei)powW, (GLsizei)powH);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBufffer);


			// if depth format is the one with stencil part, bind same render buffer as stencil attachment
			if (depthStencilFormat == GL_DEPTH24_STENCIL8)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBufffer);
			}
		}*/

		sprite = Sprite::createWithTexture(*_texture);

		glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);

		ret = true;
	} while (0);

	CC_SAFE_FREE(data);

	return ret;
}

void RenderTexture::onBegin()
{
	Director *director = Director::getInstance();

	_oldProjMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _projectionMatrix);

	_oldTransMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _transformMatrix);

	if (!_keepMatrix)
	{
//		director->setProjection(director->getProjection());


		const Size& texSize = _texture->getContentSizeInPixel();

		// Calculate the adjustment ratios based on the old and new projections
		Size size = director->getWinSize();
		float widthRatio = size.width / texSize.width;
		float heightRatio = size.height / texSize.height;

	//	Mat4 orthoMatrix;
	//	Mat4::createOrthographicOffCenter((float)-1.0 / widthRatio, (float)1.0 / widthRatio, (float)-1.0 / heightRatio, (float)1.0 / heightRatio, -1, 1, &orthoMatrix);
	//	director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);
	}
	else
	{
	}

	//calculate viewport
	{
		float viewPortRectWidthRatio = 1;
		float viewPortRectHeightRatio = 1;
		float x = 0;
		float y = 0;
		//glViewport(_fullviewPort.origin.x, _fullviewPort.origin.y, (GLsizei)_fullviewPort.size.width, (GLsizei)_fullviewPort.size.height);
		glViewport(x, y, (GLsizei)_texture->getContentSizeInPixel().width, (GLsizei)_texture->getContentSizeInPixel().height);
	}

	// Adjust the orthographic projection and viewport

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

}

void RenderTexture::onEnd()
{
	Director *director = Director::getInstance();

	glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);

	
	// restore viewport
//	director->setViewport();
	glViewport(0, 0, (GLsizei)director->getWinSize().width, (GLsizei)director->getWinSize().height);
	//
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _oldProjMatrix);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _oldTransMatrix);
}

void RenderTexture::begin()
{
	Director* director = Director::getInstance();

//	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	_projectionMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

//	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_transformMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	
}

void RenderTexture::end()
{
//	Director* director = Director::getInstance();
	//director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
//	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void RenderTexture::saveToFile(std::string filename)
{
	Image* image = newImage();
	if (image)
	{
		image->saveImageToPNG(filename.c_str(), false);
	}
	delete image;
	image = nullptr;
}

Image* RenderTexture::newImage()
{

	if (nullptr == _texture)
	{
		return nullptr;
	}

	const Size& s = _texture->getContentSizeInPixel();

	// to get the image size to save
	//        if the saving image domain exceeds the buffer texture domain,
	//        it should be cut
	int savedBufferWidth = (int)s.width;
	int savedBufferHeight = (int)s.height;

	GLubyte *buffer = nullptr;
	GLubyte *tempData = nullptr;
	Image *image = new (std::nothrow) Image();

	do
	{
		CC_BREAK_IF(!(buffer = new (std::nothrow) GLubyte[savedBufferWidth * savedBufferHeight * 4]));

		if (!(tempData = new (std::nothrow) GLubyte[savedBufferWidth * savedBufferHeight * 4]))
		{
			delete[] buffer;
			buffer = nullptr;
			break;
		}

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

		// TODO: move this to configration, so we don't check it every time
		/*  Certain Qualcomm Andreno gpu's will retain data in memory after a frame buffer switch which corrupts the render to the texture. The solution is to clear the frame buffer before rendering to the texture. However, calling glClear has the unintended result of clearing the current texture. Create a temporary texture to overcome this. At the end of RenderTexture::begin(), switch the attached texture to the second one, call glClear, and then switch back to the original texture. This solution is unnecessary for other devices as they don't have the same issue with switching frame buffers.
		*/
	
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, savedBufferWidth, savedBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, tempData);
		glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);

		bool fliimage = true;
		if (fliimage) // -- flip is only required when saving image to file
		{
			// to get the actual texture data
			// #640 the image read from rendertexture is dirty
			for (int i = 0; i < savedBufferHeight; ++i)
			{
				memcpy(&buffer[i * savedBufferWidth * 4],
					&tempData[(savedBufferHeight - i - 1) * savedBufferWidth * 4],
					savedBufferWidth * 4);
			}

			image->initWithRawData(buffer, savedBufferWidth * savedBufferHeight * 4, savedBufferWidth, savedBufferHeight, 8);
		}
		else
		{
			image->initWithRawData(tempData, savedBufferWidth * savedBufferHeight * 4, savedBufferWidth, savedBufferHeight, 8);
		}

	} while (0);

	CC_SAFE_DELETE_ARRAY(buffer);
	CC_SAFE_DELETE_ARRAY(tempData);

	return image;
}

RenderTexture::~RenderTexture()
{
	if (_FBO)
	{
		glDeleteFramebuffers(1, &_FBO);
	}

	if (_depthRenderBufffer)
	{
		glDeleteRenderbuffers(1, &_depthRenderBufffer);
	}
}