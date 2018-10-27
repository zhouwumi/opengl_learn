#ifndef  _RENDER_TEXTURE_H
#define  _RENDER_TEXTURE_H

#include <string>

#include "GL/glew.h"
#include "glfw3.h"

#include "CCTexture2D.h"
#include "Sprite.h"

class RenderTexture{
public:

	static RenderTexture* create(int width, int height);

	RenderTexture();

	~RenderTexture();

	bool init(int width, int height);
	void begin();

	void end();

	void saveToFile(std::string filename);

	void onBegin();
	void onEnd();

	Image* newImage();

private:
	GLuint       _FBO;
	GLuint       _depthRenderBufffer;
	GLint        _oldFBO;
	Texture2D* _texture;
	Texture2D::PixelFormat _pixelFormat;

	Sprite* sprite;
	Mat4 _oldTransMatrix, _oldProjMatrix;
	Mat4 _transformMatrix, _projectionMatrix;
	bool _keepMatrix;
};

#endif