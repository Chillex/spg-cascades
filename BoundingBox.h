#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "Shader.h"

class Ray;

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(glm::vec3 min, glm::vec3 max);
	~BoundingBox();

	void Expand(BoundingBox* other);
	int GetLongestAxis() const;
	void Draw(Shader* shader) const;

	bool Intersects(const Ray& ray, float t0, float t1);

	bool highlight;

protected:
	GLuint VAO;
	GLuint VBO;

	glm::vec3 bounds[2];
};

#endif