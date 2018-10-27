#include "ClippingNode.h"
#include "CommonMacros.h"
#include "Director.h"
#include "render/GLProgram.h"

#include "render/ccShader_PositionTexture_uColor.vert"
#include "render/ccShader_PositionTexture_uColor.frag"


GLint ClippingNode::s_layer = -1;

ClippingNode::ClippingNode()
	:_inverted(false)
	, _alphaThreshold(1.0)
	, _currentStencilEnabled(GL_FALSE)
	, _currentStencilWriteMask(~0)
	, _currentStencilFunc(GL_ALWAYS)
	, _currentStencilRef(0)
	, _currentStencilValueMask(~0)
	, _currentStencilFail(GL_KEEP)
	, _currentStencilPassDepthFail(GL_KEEP)
	, _currentStencilPassDepthPass(GL_KEEP)
	, _currentDepthWriteMask(GL_TRUE)
	, _currentAlphaTestEnabled(GL_FALSE)
	, _currentAlphaTestFunc(GL_ALWAYS)
	, _currentAlphaTestRef(1)
{

}


ClippingNode* ClippingNode::create()
{
	ClippingNode* node = new (std::nothrow) ClippingNode();
	if (!node->init())
	{
		CC_SAFE_DELETE(node);
	}
	return node;
}

ClippingNode* ClippingNode::create(Sprite* pStencil)
{
	ClippingNode* node = new (std::nothrow) ClippingNode();
	if (!node->init(pStencil))
	{
		CC_SAFE_DELETE(node);
	}
	return node;
}

void ClippingNode::initGLProgram()
{
	glProgram = new (std::nothrow) GLProgram();
	glProgram->initWithSource(ccPositionTexture_uColor_vert, ccPositionTexture_uColor_frag);
}

bool ClippingNode::init()
{
	return init(nullptr);
}

bool ClippingNode::init(Sprite* pStencil)
{
	m_stencil = pStencil;
	initGLProgram();
	GLint g_sStencilBits;
	static bool once = true;
	if (once)
	{
		glGetIntegerv(GL_STENCIL_BITS, &g_sStencilBits);
		if (g_sStencilBits <= 0)
		{
			CCLOG("Stencil buffer is not enabled.");
		}
		once = false;
	}
	return true;
}

void ClippingNode::setSprite(Sprite* pSelf)
{
	self = pSelf;
}

void ClippingNode::setStencil(Sprite* stencil)
{
	m_stencil = stencil;
}

void ClippingNode::setInverse(bool inverse)
{
	_inverted = inverse;
}

void ClippingNode::setAlpha(float alpha)
{
	_alphaThreshold = alpha;
}

void ClippingNode::onBeforeVisit()
{

	s_layer++;
	mask_layer = 0x1 << s_layer;

	GLint mask_layer_l = mask_layer - 1;
	mask_layer_le = mask_layer_l | mask_layer;


	_currentStencilEnabled = glIsEnabled(GL_STENCIL_TEST);

	//先保存一系列模板测试的状态，
	glGetIntegerv(GL_STENCIL_WRITEMASK, (GLint *)&_currentStencilWriteMask);

	glGetIntegerv(GL_STENCIL_FUNC, (GLint *)&_currentStencilFunc);
	glGetIntegerv(GL_STENCIL_REF, &_currentStencilRef);
	glGetIntegerv(GL_STENCIL_VALUE_MASK, (GLint *)&_currentStencilValueMask);
	glGetIntegerv(GL_STENCIL_FAIL, (GLint *)&_currentStencilFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, (GLint *)&_currentStencilPassDepthFail);
	glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, (GLint *)&_currentStencilPassDepthPass);


	glEnable(GL_STENCIL_TEST);  //开启模板测试

	glStencilMask(mask_layer); //设置模板填充值

	glGetBooleanv(GL_DEPTH_WRITEMASK, &_currentDepthWriteMask);

	glDepthMask(GL_FALSE);

	glStencilFunc(GL_NEVER, mask_layer, mask_layer);  //总不通过

	glStencilOp(!_inverted ? GL_ZERO : GL_REPLACE, GL_KEEP, GL_KEEP);

	drawFullScreenQuadClearStencil();  //填充模板缓冲区的值。

	glStencilFunc(GL_NEVER, mask_layer, mask_layer);
	glStencilOp(!_inverted ? GL_REPLACE : GL_ZERO, GL_KEEP, GL_KEEP);

	if (_alphaThreshold < 1.0) //alpha测试 是为了裁掉模板区域的透明区域。
	{
		//保存alpha测试的一些状态
		_currentAlphaTestEnabled = glIsEnabled(GL_ALPHA_TEST);
		glGetIntegerv(GL_ALPHA_TEST_FUNC, (GLint *)&_currentAlphaTestFunc);
		glGetFloatv(GL_ALPHA_TEST_REF, &_currentAlphaTestRef);
		// enable alpha testing
		glEnable(GL_ALPHA_TEST);
		// pixel will be drawn only if greater than an alpha threshold
		glAlphaFunc(GL_GREATER, _alphaThreshold);
	}
}

void ClippingNode::drawFullScreenQuadClearStencil()
{

	Director* director = Director::getInstance();

	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	Vec2 vertices[] = {
		Vec2(-1, -1),
		Vec2(1, -1),
		Vec2(1, 1),
		Vec2(-1, 1)
	};

	int colorLocation = glProgram->getUniformLocation("u_color");

	Color4F color(1.0, 1.0, 1.0, 1);
	glUseProgram(glProgram->shaderProgram);

	glProgram->setUniformsForBuiltins();

	glUniform4fv(colorLocation, (GLsizei)1, &color.r);
//	glUniform4f(colorLocation, color.r, color.g, color.b, color.a); 

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);

	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void ClippingNode::onAfterVisit()
{

	glStencilMask(_currentStencilWriteMask);
	glStencilFunc(_currentStencilFunc, _currentStencilRef, _currentStencilValueMask);

	glStencilOp(_currentStencilFail, _currentStencilPassDepthFail, _currentStencilPassDepthPass);

	

	if (!_currentStencilEnabled)
	{
		glDisable(GL_STENCIL_TEST);
	}

	s_layer--;
}

void ClippingNode::onAfterVisitStencil()
{

	if (_alphaThreshold < 1.0)  //恢复以前的alpha测试的状态
	{
		glAlphaFunc(_currentAlphaTestFunc, _currentAlphaTestRef);
		if (!_currentAlphaTestEnabled)
		{
			glDisable(GL_ALPHA_TEST);
		}
	}

	// restore the depth test state
	glDepthMask(_currentDepthWriteMask);

	glStencilFunc(GL_EQUAL, mask_layer, mask_layer);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

}

void ClippingNode::visit(Render* render)
{
	
	onBeforeVisit();

	m_stencil->visit(render);

	onAfterVisitStencil(); 

	self->visit(render);
	
	onAfterVisit();
}

ClippingNode::~ClippingNode()
{
	
	CC_SAFE_DELETE(self);
	CC_SAFE_DELETE(m_stencil);
	CC_SAFE_DELETE(glProgram);
}