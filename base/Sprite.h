#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <string>
#include "render/GLProgram.h"
#include "render/CCTexture2D.h"
#include "render/Render.h"

class Sprite
{
public:
	Sprite();

	static Sprite* create();
	static Sprite* createWithFileName(std::string filename);
	static Sprite* createWithTexture(Texture2D texture);
	virtual bool init();
	virtual bool initWithFileName(std::string filename);

	void visit(Render* render);

	void setPosition(const Vec2& position);
	void setPosition(float x, float y);

	inline void setName(std::string name){ _name = name; }
	inline std::string getName(){ return _name; }

	void setRotation(float rotationX, float rotationY);
	void setRotationX(float rotationX);
	void setRotationY(float rotationY);

	void setScale(float scale);
	void setScaleX(float scaleX);
	void setScaleY(float scaleY);

	void updateTransform();
private:
	void initShaderProgram();

private:
	GLProgram m_glProgram;
	Texture2D texture2d;
	QuadCommand _quadCommand;
	Vec2 _position;
	float _rotationX, _rotationY;
	float _scaleX, _scaleY;
	std::string _name;

	Mat4 _transform;
	Quaternion _rotationQuat;

	BlendFunc        _blendFunc;          

};

#endif