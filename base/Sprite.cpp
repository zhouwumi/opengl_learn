#include "Sprite.h"
#include "CommonMacros.h"

#include "render/ccShader_PositionTextureColor_noMVP.frag"
#include "render/ccShader_PositionTextureColor_noMVP.vert"

#include "CCImage.h"
#include "FileUtils.h"
#include "Director.h"

Sprite::Sprite():
_rotationX(0),
_rotationY(0),
_scaleX(1.0f),
_scaleY(1.0f)
{

}

Sprite* Sprite::createWithFileName(std::string filename)
{
	Sprite* sprite = new (std::nothrow) Sprite();
	if (!sprite->initWithFileName(filename))
	{
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	return sprite;
}

Sprite* Sprite::create()
{
	Sprite* sprite = new (std::nothrow) Sprite();
	if (!sprite->init())
	{
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	return sprite;
}

Sprite* Sprite::createWithTexture(Texture2D texture)
{
	Sprite* sprite = new (std::nothrow) Sprite();
	sprite->texture2d = texture;
	return sprite;
}

bool Sprite::init()
{
	return true;
}

bool Sprite::initWithFileName(std::string filename)
{
	if (!FileUtils::getInstance()->isFileExist(filename))
	{
		CCLOGMSG("can find file %s", filename);
		return false;
	}

	_blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
	Image image;
	image.initWithImageFile(filename);
	texture2d.initWithImage(&image);

	initShaderProgram();
	return true;
}



void Sprite::initShaderProgram()
{
	this->m_glProgram.initWithSource(ccPositionTextureColor_noMVP_vert, ccPositionTextureColor_noMVP_frag);
}

void Sprite::setRotation(float rotationX, float rotationY)
{
	_rotationX = rotationX;
	_rotationY = rotationY;
}

void Sprite::setRotationX(float rotationX)
{
	_rotationX = rotationX; 
}

void Sprite::setRotationY(float rotationY)
{
	_rotationY = rotationY;
}

void Sprite::setScale(float scale)
{
	_scaleX = _scaleY = scale; 
}

void Sprite::setScaleX(float scaleX)
{
	_scaleX = scaleX;
}

void Sprite::setScaleY(float scaleY)
{
	_scaleY = scaleY;
}

void Sprite::updateTransform()
{

	float halfRadx = CC_DEGREES_TO_RADIANS(_rotationX / 2.f), halfRady = CC_DEGREES_TO_RADIANS(_rotationY / 2.f), halfRadz = 0;
	float coshalfRadx = cosf(halfRadx), sinhalfRadx = sinf(halfRadx), coshalfRady = cosf(halfRady), sinhalfRady = sinf(halfRady), coshalfRadz = cosf(halfRadz), sinhalfRadz = sinf(halfRadz);
	_rotationQuat.x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
	_rotationQuat.y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
	_rotationQuat.z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
	_rotationQuat.w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;


	Mat4::createRotation(_rotationQuat, &_transform);

	if (_scaleX != 1.f)
	{
		_transform.m[0] *= _scaleX, _transform.m[1] *= _scaleX, _transform.m[2] *= _scaleX;
	}
	if (_scaleY != 1.f)
	{
		_transform.m[4] *= _scaleY, _transform.m[5] *= _scaleY, _transform.m[6] *= _scaleY;
	}

}


void Sprite::visit(Render* render)
{
	updateTransform();
	V3F_C4B_T2F_Quad quads;

	quads.bl.colors = Color4B::WHITE;
	quads.br.colors = Color4B::WHITE;
	quads.tl.colors = Color4B::WHITE;
	quads.tr.colors = Color4B::WHITE;

	float atlasWidth = (float)texture2d.getPixelsWide();
	float atlasHeight = (float)texture2d.getPixelsHigh();
	Size winSize = Director::getInstance()->getWinSize();

	Rect rect(0, 0, atlasWidth, atlasHeight);
	//float x1 = 2 * (_position.x / winSize.width) - 1;
	//float y1 = 2*(_position.y/winSize.height)-1;
	//float x2 = 2*(_position.x + atlasWidth)/960 -1;
	//float y2 = 2*(_position.y + atlasHeight)/640 - 1;
	float z = -9;
	if (_name.compare("sprite2") == 0)
	{
		z = -10;
	}
	float x1 = _position.x;
	float y1 = _position.y;
	float x2 = _position.x + atlasWidth;
	float y2 = _position.y + atlasHeight;

	quads.bl.vertices = Vec3(x1, y1, z);
	quads.br.vertices = Vec3(x2, y1, z);
	quads.tl.vertices = Vec3(x1, y2, z);
	quads.tr.vertices = Vec3(x2, y2, z);

	float left, right, top, bottom;

	left = rect.origin.x / atlasWidth;
	right = (rect.origin.x + rect.size.width) / atlasWidth;
	top = rect.origin.y / atlasHeight;
	bottom = (rect.origin.y + rect.size.height) / atlasHeight;

	quads.bl.texCoords.u = left;
	quads.bl.texCoords.v = bottom;
	quads.br.texCoords.u = right;
	quads.br.texCoords.v = bottom;
	quads.tl.texCoords.u = left;
	quads.tl.texCoords.v = top;
	quads.tr.texCoords.u = right;
	quads.tr.texCoords.v = top;

	_quadCommand.init(&quads, 1, texture2d, m_glProgram, _transform, _blendFunc);
	render->putQuadCmd(&_quadCommand);
}

void Sprite::setPosition(const Vec2& position)
{
	_position.x = position.x;
	_position.y = position.y;
}

void Sprite::setPosition(float x, float y)
{
	setPosition(Vec2(x, y));
}