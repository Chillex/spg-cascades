#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include <glm/detail/type_vec3.hpp>
#include <GL/glew.h>

class Shader;

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(glm::vec3 minP, glm::vec3 maxP);

	uint32_t GetLongestAxis(void) const;
	void Draw(Shader* shader) const;

	glm::vec3 mMin, mMax;
	bool highlight;

private:
	GLuint m_vao;
	GLuint m_vbo;

	void CreateVAO(void);
};

#endif