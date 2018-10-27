#ifndef  __QUAD_COMMAND_H_
#define __QUAD_COMMAND_H_

#include "render/CCTexture2D.h"
#include "render/GLProgram.h"
#include "ccTypes.h"


class QuadCommand{
public:
	QuadCommand();
	~QuadCommand();

	void init(V3F_C4B_T2F_Quad* quads, size_t quadsCount,Texture2D texture2d,GLProgram _glProgram,const Mat4& _mv,BlendFunc _blendFunc);

	inline V3F_C4B_T2F_Quad* getQuads() const { return _quads; }
	inline ssize_t getQuadCount() const { return _quadsCount; }

	void useMaterial();
private:
	size_t _quadsCount;
	V3F_C4B_T2F_Quad* _quads;

	GLProgram m_glProgram;

	Texture2D _texture2d;
	Mat4 mv;

	BlendFunc blendFunc;
};

#endif