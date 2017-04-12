#ifndef QUAD_H
#define QUAD_H

#include <GL/glew.h>
#include <glm/glm.hpp>

class Quad
{
public:
	Quad();
	~Quad();

	void Render(void) const;
	
	glm::mat4 GetModelMatrix(void) const;
private:
	GLuint m_vao;
	GLuint m_vbo;
};

#endif