#ifndef  _CC_GL_STATE_MANAGER_H_
#define  _CC_GL_STATE_MANAGER_H_

#include "GL/glew.h"
#include "glfw3.h"

namespace GL{

	static GLenum s_blendingSource = -1;
	static GLenum s_blendingTarget = -1;

	void setBlend(GLenum tfactor, GLenum sfactor);

};


#endif