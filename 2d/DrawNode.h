#ifndef  _DRAW_NODE_H_
#define _DRAW_NODE_H_


#include "render/GLProgram.h"
#include "ccTypes.h"


class DrawNode
{
public:
	DrawNode();
	~DrawNode();
	static DrawNode* create();
	bool init();
	void setUpVao();
	void ensurePointCapacity(size_t count);
	void drawPoint(const Vec2& position,const float pointSize,const Color4F& color);

	void ensureLineCapacity(size_t count);
	void drawLine(const Vec2& origin, const Vec2& dest, const Color4F& color);
	void onDrawPoinit();
	void onDrawLine();

private:
	size_t _bufferGLpointCount;
	size_t  _bufferGLCapacity;
	V2F_C4F_T2F* _bufferPoints;
	GLuint _pointVao, _pointVbo;
	GLProgram* _glPointProgram;

	size_t _bufferGLLineCount; 
	size_t _bufferGLLineCapacity;
	V2F_C4F_T2F* _bufferLines;
	GLuint _lineVao, _lineVbo;
	GLProgram* _glLineProgram;

	BlendFunc   _blendFunc;
	
};

#endif