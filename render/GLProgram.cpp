#include "GLProgram.h"
#include <stdio.h>
#include <assert.h>
#include "CCGeometry.h"
#include "Director.h"


// uniform names
const char* GLProgram::UNIFORM_NAME_AMBIENT_COLOR = "CC_AmbientColor";
const char* GLProgram::UNIFORM_NAME_P_MATRIX = "CC_PMatrix";
const char* GLProgram::UNIFORM_NAME_MV_MATRIX = "CC_MVMatrix";
const char* GLProgram::UNIFORM_NAME_MVP_MATRIX = "CC_MVPMatrix";
const char* GLProgram::UNIFORM_NAME_NORMAL_MATRIX = "CC_NormalMatrix";
const char* GLProgram::UNIFORM_NAME_TIME = "CC_Time";
const char* GLProgram::UNIFORM_NAME_SIN_TIME = "CC_SinTime";
const char* GLProgram::UNIFORM_NAME_COS_TIME = "CC_CosTime";
const char* GLProgram::UNIFORM_NAME_RANDOM01 = "CC_Random01";
const char* GLProgram::UNIFORM_NAME_SAMPLER0 = "CC_Texture0";
const char* GLProgram::UNIFORM_NAME_SAMPLER1 = "CC_Texture1";
const char* GLProgram::UNIFORM_NAME_SAMPLER2 = "CC_Texture2";
const char* GLProgram::UNIFORM_NAME_SAMPLER3 = "CC_Texture3";
const char* GLProgram::UNIFORM_NAME_ALPHA_TEST_VALUE = "CC_alpha_value";

// Attribute names
const char* GLProgram::ATTRIBUTE_NAME_COLOR = "a_color";
const char* GLProgram::ATTRIBUTE_NAME_POSITION = "a_position";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD = "a_texCoord";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD1 = "a_texCoord1";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD2 = "a_texCoord2";
const char* GLProgram::ATTRIBUTE_NAME_TEX_COORD3 = "a_texCoord3";
const char* GLProgram::ATTRIBUTE_NAME_NORMAL = "a_normal";
const char* GLProgram::ATTRIBUTE_NAME_BLEND_WEIGHT = "a_blendWeight";
const char* GLProgram::ATTRIBUTE_NAME_BLEND_INDEX = "a_blendIndex";

bool GLProgram::compileShader(GLuint* shader, GLenum type, const GLchar* shaderSource)
{
	if (shaderSource == NULL)
	{
		return false;
	}
	*shader = glCreateShader(type);
	const GLchar *sources[] = {
		"uniform mat4 CC_PMatrix;\n"
		"uniform mat4 CC_MVMatrix;\n"
		"uniform mat4 CC_MVPMatrix;\n"
		"uniform mat3 CC_NormalMatrix;\n"
		"uniform vec4 CC_Time;\n"
		"uniform vec4 CC_SinTime;\n"
		"uniform vec4 CC_CosTime;\n"
		"uniform vec4 CC_Random01;\n"
		"uniform sampler2D CC_Texture0;\n"
		"uniform sampler2D CC_Texture1;\n"
		"uniform sampler2D CC_Texture2;\n"
		"uniform sampler2D CC_Texture3;\n"
		"//CC INCLUDES END\n\n",
		shaderSource,
	};
	glShaderSource(*shader, sizeof(sources)/sizeof(*sources), sources, NULL);
	glCompileShader(*shader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(*shader, 512, NULL, infoLog);
		printf(infoLog);
		return false;
		//	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return success == GL_TRUE;
}

void GLProgram::link()
{
	glAttachShader(this->shaderProgram,this->vertexShader);
	glAttachShader(this->shaderProgram, this->fragementShaer);

	glLinkProgram(this->shaderProgram);

	GLint success;
	GLchar infoLog[512];
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	GLint num = 0;
	glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTES, &num);

	
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		//	std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(this->vertexShader);
	glDeleteShader(this->fragementShaer);
}

void GLProgram::initWithSource(const char* vertexShaderSource, const char*  fragementShaderSource)
{
	this->shaderProgram = glCreateProgram();

	if (!compileShader(&this->vertexShader, GL_VERTEX_SHADER, (const GLchar*)vertexShaderSource))
	{
		return; 
	}
	
	if (!compileShader(&this->fragementShaer, GL_FRAGMENT_SHADER,(const GLchar*)fragementShaderSource))
	{
		return;
	}

	bindPredefinedVertexAttribs();

	link();

	parseAttribute();
	parseUniforms();
	updateUniforms();

	texture2d0Pos = glGetUniformLocation(shaderProgram, "CC_Texture0");
}

GLint GLProgram::getUniformLocation(const std::string& attributeName) const 
{
	return glGetUniformLocation(shaderProgram, attributeName.c_str());
}

void GLProgram::apply()
{
	glUseProgram(this->shaderProgram);
	setUniformsForBuiltins();
}

void GLProgram::apply(const Mat4&  modelView)
{
	glUseProgram(this->shaderProgram);
	setUniformsForBuiltins(modelView);
}

void GLProgram::bindPredefinedVertexAttribs()
{
	static const struct {
		const char *attributeName;
		int location;
	} attribute_locations[] =
	{
		{ GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION },
		{ GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR },
		{ GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORD },
		{ GLProgram::ATTRIBUTE_NAME_TEX_COORD1, GLProgram::VERTEX_ATTRIB_TEX_COORD1 },
		{ GLProgram::ATTRIBUTE_NAME_TEX_COORD2, GLProgram::VERTEX_ATTRIB_TEX_COORD2 },
		{ GLProgram::ATTRIBUTE_NAME_TEX_COORD3, GLProgram::VERTEX_ATTRIB_TEX_COORD3 },
		{ GLProgram::ATTRIBUTE_NAME_NORMAL, GLProgram::VERTEX_ATTRIB_NORMAL },
	};

	const int size = sizeof(attribute_locations) / sizeof(attribute_locations[0]);

	for (int i = 0; i < size; i++) {
		glBindAttribLocation(this->shaderProgram, attribute_locations[i].location, attribute_locations[i].attributeName);
	}
}

void GLProgram::parseAttribute()
{
	_vertexAttribs.clear();
	GLint activeAttributes = 0;
	glGetProgramiv(this->shaderProgram, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
	
	if (activeAttributes > 0)
	{
		GLint maxLength;
		glGetProgramiv(this->shaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

		for (int i = 0; i < activeAttributes; i++)
		{
			VertexAttrib vertexAttrib;
			GLchar* attribName = (GLchar*)alloca(maxLength + 1);
			GLint length, size;
			GLenum type;
			glGetActiveAttrib(this->shaderProgram, i, maxLength, nullptr, &vertexAttrib.size, &vertexAttrib.type, attribName);
			std::string name = attribName;
			vertexAttrib.name = name;
			GLint position = glGetAttribLocation(this->shaderProgram, attribName);
			vertexAttrib.index = position;
			_vertexAttribs[vertexAttrib.name] = vertexAttrib;
		}
	}
}

void GLProgram::parseUniforms()
{
	_useUniforms.clear();
	GLint _activeUnifroms = 0;
	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS,&_activeUnifroms);
	
	if (_activeUnifroms > 0)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
		for (int i = 0; i < _activeUnifroms;i++)
		{
			GLint size;
			GLenum type;
			GLchar* name = (GLchar*)alloca((maxLength+1)*sizeof(GLchar));
			glGetActiveUniform(shaderProgram, i, maxLength, nullptr, &size, &type, name);
			name[maxLength] = '\0';
			if (strncmp("CC_", name, 3) != 0)
			{
				Uniform uniform;
				GLint location = glGetUniformLocation(shaderProgram, name);
				uniform.location = location;
				uniform.type = type;
				uniform.size = size;
				uniform.name = std::string(name);

				GLenum __gl_error_code = glGetError();
				if (__gl_error_code != GL_NO_ERROR)
				{
					printf("error: 0x%x", (int)__gl_error_code);
				}
				assert(__gl_error_code == GL_NO_ERROR);
				_useUniforms[uniform.name] = uniform;
			}
		}
	}
}

void GLProgram::updateUniforms()
{
	_builtInUniforms[UNIFORM_AMBIENT_COLOR] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_AMBIENT_COLOR);
	_builtInUniforms[UNIFORM_P_MATRIX] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_P_MATRIX);
	_builtInUniforms[UNIFORM_MV_MATRIX] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_MV_MATRIX);
	_builtInUniforms[UNIFORM_MVP_MATRIX] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_MVP_MATRIX);
	_builtInUniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_NORMAL_MATRIX);

	_builtInUniforms[UNIFORM_TIME] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_TIME);
	_builtInUniforms[UNIFORM_SIN_TIME] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_SIN_TIME);
	_builtInUniforms[UNIFORM_COS_TIME] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_COS_TIME);

	_builtInUniforms[UNIFORM_RANDOM01] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_RANDOM01);

	_builtInUniforms[UNIFORM_SAMPLER0] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_SAMPLER0);
	_builtInUniforms[UNIFORM_SAMPLER1] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_SAMPLER1);
	_builtInUniforms[UNIFORM_SAMPLER2] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_SAMPLER2);
	_builtInUniforms[UNIFORM_SAMPLER3] = glGetUniformLocation(shaderProgram, UNIFORM_NAME_SAMPLER3);

	_flags.usesP = _builtInUniforms[UNIFORM_P_MATRIX] != -1;
	_flags.usesMV = _builtInUniforms[UNIFORM_MV_MATRIX] != -1;
	_flags.usesMVP = _builtInUniforms[UNIFORM_MVP_MATRIX] != -1;
	_flags.usesNormal = _builtInUniforms[UNIFORM_NORMAL_MATRIX] != -1;
	_flags.usesTime = (
		_builtInUniforms[UNIFORM_TIME] != -1 ||
		_builtInUniforms[UNIFORM_SIN_TIME] != -1 ||
		_builtInUniforms[UNIFORM_COS_TIME] != -1
		);
	_flags.usesRandom = _builtInUniforms[UNIFORM_RANDOM01] != -1;

}


void GLProgram::setUniformsForBuiltins()
{
	Mat4 modelView = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	setUniformsForBuiltins(modelView);
}

void GLProgram::setUniformsForBuiltins(const Mat4& modelView)
{
	Mat4 projectionMatrix, modalViewMatrix;
	projectionMatrix = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	modalViewMatrix = modelView;
	if (_flags.usesP)
	{
		glUniformMatrix4fv((GLint)_builtInUniforms[UNIFORM_P_MATRIX], (GLsizei)1, GL_FALSE, projectionMatrix.m);
	}

	if (_flags.usesMV)
	{
		glUniformMatrix4fv((GLint)_builtInUniforms[UNIFORM_MV_MATRIX], (GLsizei)1, GL_FALSE, modalViewMatrix.m);
	}

	if (_flags.usesMVP)
	{
		Mat4 mvpMatrix = projectionMatrix * modalViewMatrix;
		glUniformMatrix4fv((GLint)_builtInUniforms[UNIFORM_MVP_MATRIX], (GLsizei)1, GL_FALSE, mvpMatrix.m);
	}
}