#ifndef  __RENDER_H_
#define  __RENDER_H_

#include "GLProgram.h"
#include "ccTypes.h"
#include <vector>
#include "render/QuadCommand.h"

class Render
{
public:
	static const int VBO_SIZE = 65536;
	static const int INDEX_VBO_SIZE = VBO_SIZE * 6 / 4;

	void setUpVAo();
	void draw();
	void init();
	void putQuadCmd(QuadCommand* command);
	void fillQuadCmd(QuadCommand* command);
private:
	GLuint _quadVAO;
	GLuint _quadbuffersVBO[2];
	V3F_C4B_T2F _quadVerts[VBO_SIZE];
	GLint _quadIndices[INDEX_VBO_SIZE];

	std::vector<QuadCommand*> quadCmds;

	size_t fillVertIndex, fillIndiceIndex;

	size_t _numberQuads;
};

#endif