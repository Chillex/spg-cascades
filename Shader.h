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
	Shader(std::string vertexPath, std::string fragmentPath);
	Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath);

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
};

#endif