#include "2d/DrawNode.h"
#include "CommonMacros.h"

#include "render/ccShader_PositionColorTextureAsPointsize.vert"
#include "render/ccShader_PositionColor.frag"

#include "render/ccShader_PositionColorLengthTexture.vert"
#include "render/ccShader_PositionColorLengthTexture.frag"

#include "render/ccGLStateManager.h"

DrawNode::DrawNode()
	:_bufferPoints(nullptr)
	, _bufferGLCapacity(0)
	, _bufferGLpointCount(0)
	, _bufferGLLineCapacity(0)
	, _bufferGLLineCount(0)
	, _bufferLines(nullptr)
{

}

DrawNode::~DrawNode()
{
	if (_bufferPoints)
	{
		free(_bufferPoints);
	}

	if (_glPointProgram)
	{
		CC_SAFE_DELETE(_glPointProgram);
	}
	if (_glLineProgram)
	{
		CC_SAFE_DELETE(_glLineProgram);
	}
}

DrawNode* DrawNode::create()
{
	DrawNode* node = new (std::nothrow) DrawNode();
	if (!node->init())
	{
		CC_SAFE_DELETE(node);
	}
	return node;
}

bool DrawNode::init()
{
	ensurePointCapacity(64);
	ensureLineCapacity(256);

	_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
	_glPointProgram = new GLProgram();
	_glPointProgram->initWithSource(ccPositionColorTextureAsPointsize_vert, ccPositionColor_frag);

	_glLineProgram = new GLProgram();
	_glLineProgram->initWithSource(ccPositionColorLengthTexture_vert, ccPositionColorLengthTexture_frag);
	
	setUpVao();

	return true;
}

void DrawNode::setUpVao()
{
	glGenVertexArrays(1, &_pointVao);
	glBindVertexArray(_pointVao);

	glGenBuffers(1, &_pointVbo);
	glBindBuffer(GL_ARRAY_BUFFER, _pointVbo);
	glBufferData(GL_ARRAY_BUFFER, _bufferGLCapacity* sizeof(V2F_C4F_T2F), nullptr, GL_STREAM_DRAW);

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4F_T2F), (GLvoid*)offsetof(V2F_C4F_T2F, vertices));

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4F_T2F), (GLvoid*)offsetof(V2F_C4F_T2F, colors));

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4F_T2F), (GLvoid*)offsetof(V2F_C4F_T2F, texCoords));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &_lineVao);
	glBindVertexArray(_lineVao);

	glGenBuffers(1, &_lineVbo);
	glBindBuffer(GL_ARRAY_BUFFER, _lineVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4F_T2F)*_bufferGLLineCapacity, nullptr, GL_STREAM_DRAW);

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4F_T2F), (GLvoid*)offsetof(V2F_C4F_T2F, vertices));

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_TRUE, sizeof(V2F_C4F_T2F), (GLvoid*)offsetof(V2F_C4F_T2F, colors));

	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_TRUE, sizeof(V2F_C4F_T2F), (GLvoid*)offsetof(V2F_C4F_T2F, texCoords));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawNode::ensurePointCapacity(size_t count)
{
	if (_bufferGLpointCount + count > _bufferGLCapacity)
	{
		_bufferGLCapacity += MAX(_bufferGLCapacity, count);
		_bufferPoints = (V2F_C4F_T2F*)realloc(_bufferPoints, _bufferGLCapacity * sizeof(V2F_C4F_T2F));
	}
}

void DrawNode::drawPoint(const Vec2& position, const float pointSize, const Color4F& color)
{
	ensurePointCapacity(1);

	V2F_C4F_T2F point = { position, color, Tex2F(0, 0) };
	*(_bufferPoints + _bufferGLpointCount) = point;
	_bufferGLpointCount += 1;
}

void DrawNode::onDrawPoinit()
{
	_glPointProgram->apply();
	glBindVertexArray(_pointVao);
	glBindBuffer(GL_ARRAY_BUFFER, _pointVbo);
	glBufferData(GL_ARRAY_BUFFER, _bufferGLpointCount*sizeof(V2F_C4F_T2F), _bufferPoints, GL_STREAM_DRAW);
	glDrawArrays(GL_POINTS, 0, _bufferGLpointCount);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void DrawNode::ensureLineCapacity(size_t count)
{
	if (_bufferGLLineCount + count > _bufferGLLineCapacity)
	{
		_bufferGLLineCapacity += MAX(_bufferGLLineCapacity, count);
		_bufferLines =(V2F_C4F_T2F*) realloc(_bufferLines, _bufferGLLineCapacity* sizeof(V2F_C4F_T2F));
	}
	
}

void DrawNode::drawLine(const Vec2& origin, const Vec2& dest, const Color4F& color)
{
	ensureLineCapacity(2);
	V2F_C4F_T2F originPoint = { origin, color, Tex2F(0.0, 0.0) };
	V2F_C4F_T2F destPoint = { dest, color, Tex2F(0.0, 0.0) };

	*(_bufferLines + _bufferGLLineCount) = originPoint;
	*(_bufferLines + _bufferGLLineCount + 1) = destPoint;

	_bufferGLLineCount += 2;
}

void DrawNode::onDrawLine()
{
	GL::setBlend(_blendFunc.src, _blendFunc.dst);
	_glLineProgram->apply();
	glLineWidth(2.0);
	glBindVertexArray(_lineVao);
	glBindBuffer(GL_ARRAY_BUFFER,_lineVbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4F_T2F)* _bufferGLLineCount, _bufferLines, GL_STREAM_DRAW);
	glDrawArrays(GL_LINES, 0, _bufferGLLineCount);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}