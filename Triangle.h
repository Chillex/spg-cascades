#pragma once
#include <glm/detail/type_vec3.hpp>

class Triangle
{
public:
	Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

	const glm::vec3& GetCentroid() const { return m_centroid; }

	glm::vec3 A;
	glm::vec3 B;
	glm::vec3 C;

	glm::vec3 m_centroid;
};
