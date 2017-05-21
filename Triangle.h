#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/detail/type_vec3.hpp>
#include "BoundingBox.h"

struct Intersection;

class Triangle
{
public:
	Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	~Triangle();

	BoundingBox* GetBoundingBox() const;
	glm::vec3 GetMidpoint() const;

	bool Intersects(const Ray& ray, Intersection& intersection) const;
protected:
	glm::vec3 m_p1;
	glm::vec3 m_p2;
	glm::vec3 m_p3;

};

#endif