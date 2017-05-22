#include "Triangle.h"

Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
	: A(p1)
	, B(p2)
	, C(p3)
	, m_centroid((A.x + B.x + C.x) / 3.0f, (A.y + B.y + C.y) / 3.0f, (A.z + B.z + C.z) / 3.0f)
{
}
