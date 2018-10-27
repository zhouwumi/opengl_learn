#ifndef _CLIPPING_NODE_H_
#define _CLIPPING_NODE_H_

#include "GL/glew.h"
#include "glfw3.h"
#include "Sprite.h"
#include "render/GLProgram.h"

class ClippingNode{
public:

	ClippingNode();
	~ClippingNode();
	static GLint  s_layer;

	static ClippingNode* create();
	static ClippingNode* create(Sprite* pStencil);
	bool init();
	bool init(Sprite* pStencil);
	void initGLProgram();

	void onBeforeVisit();
	void onAfterVisitStencil();
	void onAfterVisit();

	void setSprite(Sprite* pSelf);
	void setStencil(Sprite* stencil);

	void drawFullScreenQuadClearStencil();

	void visit(Render* render);

	void setInverse(bool inverse);
	void setAlpha(float alpha);
private:
	Sprite* self;
	Sprite* m_stencil;
	
	GLint mask_layer;
	GLint mask_layer_le;
	bool _inverted;

	GLboolean _currentStencilEnabled;
	GLuint _currentStencilWriteMask;
	GLenum _currentStencilFunc;
	GLint _currentStencilRef;
	GLuint _currentStencilValueMask;
	GLenum _currentStencilFail;
	GLenum _currentStencilPassDepthFail;
	GLenum _currentStencilPassDepthPass;
	GLboolean _currentDepthWriteMask;

	GLboolean _currentAlphaTestEnabled;
	GLenum _currentAlphaTestFunc;
	GLclampf _currentAlphaTestRef;


	GLfloat _alphaThreshold;

	GLProgram* glProgram;
};

#endif