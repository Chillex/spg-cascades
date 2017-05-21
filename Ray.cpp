#include "Ray.h"

Ray::Ray(glm::vec3 o, glm::vec3 d)
	: origin(o)
	, direction(d)
	, inverseDirection(1/d.x, 1/d.y, 1/d.z)
	, sign(inverseDirection.x < 0, inverseDirection.y < 0, inverseDirection.z < 0)
{
}

Ray::~Ray()
{
}
