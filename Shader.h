#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
	struct ShaderType
	{
		enum Enum
		{
			Vertex = 0,
			Geometry = 1,
			Fragment = 2
		};
	};

	Shader(std::string vertexPath, std::string secondShaderPath, ShaderType::Enum secondShaderType);
	Shader(std::string vertexPath, std::string secondShaderPath, ShaderType::Enum secondShaderType, const GLchar** feedbackVaryings, const GLuint feedbackCount);
	Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath);
	Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath, const GLchar** feedbackVaryings, const GLuint feedbackCount);
	Shader(std::string vertexPath, std::string tcsPath, std::string tesPath, std::string fragmentPath);

	~Shader();

	void CheckLinkStatus(void);
	void Use(void);

	GLuint program;

protected:
	const std::string GetShaderCode(const char* path);
	GLuint CreateShader(GLuint shaderType, const GLchar* shaderSource, const char* errorType);
	GLuint CreateVertexShader(const GLchar* shaderSource);
	GLuint CreateFragmentShader(const GLchar* shaderSource);
	GLuint CreateGeometryShader(const GLchar* shaderSource);
	GLuint CreateTCSShader(const GLchar* shaderSource);
	GLuint CreateTESShader(const GLchar* shaderSource);
};

#endif