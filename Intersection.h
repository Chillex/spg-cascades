#ifndef INTERSECTION_H
#define INTERSECTION_H
#include <glm/detail/type_vec3.hpp>
#include "Triangle.h"

struct Intersection
{
	float distance;
	glm::vec3 point;
	Triangle* hitTriangle;
};

#endif