#ifndef  __Director_H_
#define  __Director_H_

#include "GLView.h"
#include "CCGeometry.h"
#include <stack>
#include "render/Render.h"

enum class MATRIX_STACK_TYPE
{
	MATRIX_STACK_MODELVIEW,
	MATRIX_STACK_PROJECTION
};

class Director{
public:

	static Director* _defaultDirector;
	static Director* getInstance();

	GLView* getGLView();

	void drawScene();

	bool init();
	void initStack();
	
	Size getWinSize();

	void setWinSize(Size& winSize);

	void setProjection();

	const Mat4& getMatrix(MATRIX_STACK_TYPE type);
	void multiplyMatrix(MATRIX_STACK_TYPE type, Mat4& mat4);

	void loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat4);
	void popMatrix(MATRIX_STACK_TYPE type);
	void pushMatrix(MATRIX_STACK_TYPE type);

	void initRender();
	inline const Render& getRenderer(){ return *render; }
	void loadIdentityMatrix(MATRIX_STACK_TYPE type);
private:
private:
	GLView* _glView;
	Size _size;
	std::stack<Mat4> _modalViewMatrixStack;
	std::stack<Mat4> _projectionMatrixStack;

	Render* render;
};

#endif