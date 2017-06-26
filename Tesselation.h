#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

class ShaderLibrary;

class Tesselation
{
public:
	Tesselation(std::string shaderKey);
	~Tesselation();

	void Render(ShaderLibrary& shaderLib, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void RenderShadowPass(ShaderLibrary& shaderLib, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

protected:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ibo;
	glm::mat4 m_modelMatrix;
	std::string m_shaderKey;
};

