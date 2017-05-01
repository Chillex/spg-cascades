#ifndef ADVANCED_QUAD_H
#define ADVANCED_QUAD_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader;

class AdvancedQuad
{
public:
	AdvancedQuad(glm::vec3 position, const char* diffusePath, const char* normalPath, const char* displacementPath);
	~AdvancedQuad();

	void Render(Shader* shader) const;

	glm::mat4 GetModelMatrix() const;
private:
	glm::vec3 m_position;

	GLuint m_vao;
	GLuint m_vbo;

	GLuint m_diffuse;
	GLuint m_normal;
	GLuint m_displacement;

	GLuint loadTexture(const char* path);
};

#endif
