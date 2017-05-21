#include "Triangle.h"
#include "Ray.h"

#include <glm/gtc/constants.hpp>
#include "Intersection.h"

Triangle::Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
	: m_p1(p1)
	, m_p2(p2)
	, m_p3(p3)
{
}

Triangle::~Triangle()
{
}

BoundingBox* Triangle::GetBoundingBox() const
{
	glm::vec3 min = m_p1;
	if (m_p2.x < min.x) min.x = m_p2.x;
	if (m_p2.y < min.y) min.y = m_p2.y;
	if (m_p2.z < min.z) min.z = m_p2.z;
	if (m_p3.x < min.x) min.x = m_p3.x;
	if (m_p3.y < min.y) min.y = m_p3.y;
	if (m_p3.z < min.z) min.z = m_p3.z;

	glm::vec3 max = m_p1;
	if (m_p2.x > max.x) max.x = m_p2.x;
	if (m_p2.y > max.y) max.y = m_p2.y;
	if (m_p2.z > max.z) max.z = m_p2.z;
	if (m_p3.x > max.x) max.x = m_p3.x;
	if (m_p3.y > max.y) max.y = m_p3.y;
	if (m_p3.z > max.z) max.z = m_p3.z;

	return new BoundingBox(min, max);
}

glm::vec3 Triangle::GetMidpoint() const
{
	return (m_p1 + m_p2 + m_p3) / 3.0f;
}

// http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool Triangle::Intersects(const Ray& ray, Intersection& intersection) const
{
	// find vectors for the two edges, sharing p1
	glm::vec3 edge1 = m_p2 - m_p1;
	glm::vec3 edge2 = m_p3 - m_p1;

	// beginn calculating determinant
	glm::vec3 pvec = glm::cross(ray.direction, edge2);
	float det = glm::dot(edge1, pvec);

	if (det < 0.000001f)
		return false;

	// calculate distance from p1 to ray origin
	glm::vec3 tvec = ray.origin - m_p1;

	// calculate u to test bounds
	float u = glm::dot(tvec, pvec);
	if (u < 0.0f || u > det)
		return false;

	// prepare to test V parameter
	glm::vec3 qvec = glm::cross(tvec, edge1);

	// calculate v to test bounds
	float v = glm::dot(ray.direction, qvec);
	if (v < 0.0f || u + v > det)
		return false;

	// calculate t, scale params, ray intersects triangle
	float t = glm::dot(edge2, qvec);

	float invDet = 1.0f / det;
	t *= invDet;
	u *= invDet;
	v *= invDet;

	intersection.distance = t;

	return true;
}
