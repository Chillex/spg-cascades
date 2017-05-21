#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class Ray
{
public:
	Ray(glm::vec3 o, glm::vec3 d);
	~Ray();

	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 inverseDirection;
	glm::ivec3 sign;
};

#endif
