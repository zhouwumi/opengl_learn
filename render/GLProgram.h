#ifndef  __GL_PROGRAM_H_
#define __GL_PROGRAM_H_

#include "GL/glew.h"
#include "glfw3.h"
#include <string>
#include <unordered_map>
#include "Mat4.h"

struct VertexAttrib
{
	GLuint index;
	GLint size;
	GLenum type;
	std::string name;
};

struct Uniform
{
	GLuint location;
	GLuint size;
	GLenum type;
	std::string name;
};

class GLProgram{
public:

	enum
	{
		UNIFORM_AMBIENT_COLOR,
		UNIFORM_P_MATRIX,
		UNIFORM_MV_MATRIX,
		UNIFORM_MVP_MATRIX,
		UNIFORM_NORMAL_MATRIX,
		UNIFORM_TIME,
		UNIFORM_SIN_TIME,
		UNIFORM_COS_TIME,
		UNIFORM_RANDOM01,
		UNIFORM_SAMPLER0,
		UNIFORM_SAMPLER1,
		UNIFORM_SAMPLER2,
		UNIFORM_SAMPLER3,

		UNIFORM_MAX,
	};


	enum
	{
		VERTEX_ATTRIB_POSITION,
		VERTEX_ATTRIB_COLOR,
		VERTEX_ATTRIB_TEX_COORD,
		VERTEX_ATTRIB_TEX_COORD1,
		VERTEX_ATTRIB_TEX_COORD2,
		VERTEX_ATTRIB_TEX_COORD3,
		VERTEX_ATTRIB_NORMAL,
		VERTEX_ATTRIB_BLEND_WEIGHT,
		VERTEX_ATTRIB_BLEND_INDEX,
		VERTEX_ATTRIB_MAX,

		// backward compatibility
		VERTEX_ATTRIB_TEX_COORDS = VERTEX_ATTRIB_TEX_COORD,
	};

	// uniform names
	static const char* UNIFORM_NAME_AMBIENT_COLOR;
	static const char* UNIFORM_NAME_P_MATRIX;
	static const char* UNIFORM_NAME_MV_MATRIX;
	static const char* UNIFORM_NAME_MVP_MATRIX;
	static const char* UNIFORM_NAME_NORMAL_MATRIX;
	static const char* UNIFORM_NAME_TIME;
	static const char* UNIFORM_NAME_SIN_TIME;
	static const char* UNIFORM_NAME_COS_TIME;
	static const char* UNIFORM_NAME_RANDOM01;
	static const char* UNIFORM_NAME_SAMPLER0;
	static const char* UNIFORM_NAME_SAMPLER1;
	static const char* UNIFORM_NAME_SAMPLER2;
	static const char* UNIFORM_NAME_SAMPLER3;
	static const char* UNIFORM_NAME_ALPHA_TEST_VALUE;

	// Attribute names
	static const char* ATTRIBUTE_NAME_COLOR;
	static const char* ATTRIBUTE_NAME_POSITION;
	static const char* ATTRIBUTE_NAME_TEX_COORD;
	static const char* ATTRIBUTE_NAME_TEX_COORD1;
	static const char* ATTRIBUTE_NAME_TEX_COORD2;
	static const char* ATTRIBUTE_NAME_TEX_COORD3;
	static const char* ATTRIBUTE_NAME_NORMAL;
	static const char* ATTRIBUTE_NAME_BLEND_WEIGHT;
	static const char* ATTRIBUTE_NAME_BLEND_INDEX;
public:
	void initWithSource(const char* vertexShaderSource, const char* fragementShaderSource);
	bool compileShader(GLuint* shader, GLenum type, const GLchar* shaderSource);
	void link();
	void apply(const Mat4& modelView);
	void apply();
	void bindPredefinedVertexAttribs();

	void parseAttribute();
	void parseUniforms();
	void updateUniforms();

	void setUniformsForBuiltins();
	void setUniformsForBuiltins(const Mat4& modelView);

	GLint getUniformLocation(const std::string& attributeName) const;
public:
	GLuint shaderProgram,texture2d0Pos;

private:
	std::unordered_map<std::string, VertexAttrib> _vertexAttribs;
	std::unordered_map<std::string, Uniform> _useUniforms;
	GLuint  vertexShader, fragementShaer;
	GLint _builtInUniforms[UNIFORM_MAX];

	struct flag_struct {
		unsigned int usesTime : 1;
		unsigned int usesNormal : 1;
		unsigned int usesMVP : 1;
		unsigned int usesMV : 1;
		unsigned int usesP : 1;
		unsigned int usesRandom : 1;
		// handy way to initialize the bitfield
		flag_struct() { memset(this, 0, sizeof(*this)); }
	} _flags;

};

#endif