#include "render/Render.h"

void Render::init()
{
	for (int i = 0; i < VBO_SIZE / 4; i++)
	{
		_quadIndices[i * 6 + 0] = (GLushort)(i * 4 + 0);
		_quadIndices[i * 6 + 1] = (GLushort)(i * 4 + 1);
		_quadIndices[i * 6 + 2] = (GLushort)(i * 4 + 2);
		_quadIndices[i * 6 + 3] = (GLushort)(i * 4 + 3);
		_quadIndices[i * 6 + 4] = (GLushort)(i * 4 + 2);
		_quadIndices[i * 6 + 5] = (GLushort)(i * 4 + 1);
	}
	_numberQuads = 0;
	setUpVAo();
}

void Render::draw()
{

	//Bind VAO
	glBindVertexArray(_quadVAO);
	//Set VBO data
	glBindBuffer(GL_ARRAY_BUFFER, _quadbuffersVBO[0]);

	// option 1: subdata
	//        glBufferSubData(GL_ARRAY_BUFFER, sizeof(_quads[0])*start, sizeof(_quads[0]) * n , &_quads[start] );

	// option 2: data
	//        glBufferData(GL_ARRAY_BUFFER, sizeof(quads_[0]) * (n-start), &quads_[start], GL_DYNAMIC_DRAW);

	// option 3: orphaning + glMapBuffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(_quadVerts[0]) * _numberQuads * 4, nullptr, GL_DYNAMIC_DRAW);
	void *buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(buf, _quadVerts, sizeof(_quadVerts[0])* _numberQuads * 4);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadbuffersVBO[1]);

	size_t startIndex=0,indexToDraw = 0;

	for (auto cmd : quadCmds)
	{
		if (indexToDraw >0)
		{
			glDrawElements(GL_TRIANGLES, (GLsizei)indexToDraw, GL_UNSIGNED_INT, (GLvoid*)(startIndex*sizeof(_quadIndices[0])));
			startIndex += indexToDraw;
			indexToDraw = 0;
		}
		cmd->useMaterial();
		indexToDraw += cmd->getQuadCount() * 6;
		
	}

	if (indexToDraw > 0)
	{
		glDrawElements(GL_TRIANGLES, (GLsizei)indexToDraw, GL_UNSIGNED_INT, (GLvoid*)(startIndex*sizeof(_quadIndices[0])));
	}

	

	glBindVertexArray(0);

	_numberQuads = 0;
	quadCmds.clear();

}

void Render::putQuadCmd(QuadCommand* command)
{
	quadCmds.push_back(command);
	fillQuadCmd(command);
}

void Render::fillQuadCmd(QuadCommand* command)
{
	V3F_C4B_T2F* quads = (V3F_C4B_T2F*)command->getQuads();
	for (size_t i = 0; i < command->getQuadCount() * 4; i++)
	{
		_quadVerts[i + _numberQuads * 4] = quads[i];
	}
	_numberQuads += command->getQuadCount();
}

void Render::setUpVAo()
{
	glGenVertexArrays(1, &_quadVAO);
	glBindVertexArray(_quadVAO);

	glGenBuffers(2, &_quadbuffersVBO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _quadbuffersVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_quadVerts[0]) * VBO_SIZE, _quadVerts, GL_DYNAMIC_DRAW);

	// vertices
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, vertices));

	// colors
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, colors));

	// tex coords
	glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*)offsetof(V3F_C4B_T2F, texCoords));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadbuffersVBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_quadIndices[0]) * INDEX_VBO_SIZE, _quadIndices, GL_STATIC_DRAW);

	// Must unbind the VAO before changing the element buffer.
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}