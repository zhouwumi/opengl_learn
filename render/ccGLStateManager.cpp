#include "ccGLStateManager.h"

namespace GL
{
	
	static void setBlending(GLenum sfactor, GLenum tfactor)
	{
		if (tfactor == GL_ZERO && sfactor == GL_ONE)
		{
			glDisable(GL_BLEND);
		}
		else{
			glEnable(GL_BLEND);
			glBlendFunc(sfactor, tfactor);
		}
	}

	void setBlend(GLenum sfactor, GLenum tfactor)
	{
		if (sfactor != GL::s_blendingSource && tfactor != GL::s_blendingTarget)
		{
			GL::s_blendingSource = sfactor;
			GL::s_blendingTarget = tfactor;
			setBlending(sfactor, tfactor);
		}
	}

}