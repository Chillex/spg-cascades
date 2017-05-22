#pragma once
#include "BoundingBox.h"
#include "Ray.h"
#include "Triangle.h"
#include <glm/glm.hpp>

inline bool RayBoxIntersection(const Ray& ray, const BoundingBox& aabb)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	int bound = ray.mSign[0];

	if (bound == 0)
	{
		tmin = (aabb.mMin.x - ray.mOrigin.x) * ray.mInvDir.x;
		tmax = (aabb.mMax.x - ray.mOrigin.x) * ray.mInvDir.x;
	}
	else
	{
		tmin = (aabb.mMax.x - ray.mOrigin.x) * ray.mInvDir.x;
		tmax = (aabb.mMin.x - ray.mOrigin.x) * ray.mInvDir.x;
	}

	bound = ray.mSign[1];

	if (bound == 0)
	{
		tymin = (aabb.mMin.y - ray.mOrigin.y) * ray.mInvDir.y;
		tymax = (aabb.mMax.y - ray.mOrigin.y) * ray.mInvDir.y;
	}
	else
	{
		tymin = (aabb.mMax.y - ray.mOrigin.y) * ray.mInvDir.y;
		tymax = (aabb.mMin.y - ray.mOrigin.y) * ray.mInvDir.y;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	bound = ray.mSign[2];

	if (bound == 0)
	{
		tzmin = (aabb.mMin.z - ray.mOrigin.z) * ray.mInvDir.z;
		tzmax = (aabb.mMax.z - ray.mOrigin.z) * ray.mInvDir.z;
	}
	else
	{
		tzmin = (aabb.mMax.z - ray.mOrigin.z) * ray.mInvDir.z;
		tzmax = (aabb.mMin.z - ray.mOrigin.z) * ray.mInvDir.z;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;
}

#define EPSILON 0.000001

inline bool RayTriangleIntersection(const Ray& ray, Triangle& tri, float& hitDist)
{
	glm::vec3 e1, e2;  //Edge1, Edge2
	glm::vec3 P, Q, T;
	float det, inv_det, u, v;
	float t;

	//Find vectors for two edges sharing V1
	e1 = tri.B - tri.A;
	e2 = tri.C - tri.A;
	//Begin calculating determinant - also used to calculate u parameter
	P = glm::cross(ray.mDir, e2);
	//if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	det = glm::dot(e1, P);
	//NOT CULLING
	if (det < EPSILON) return false;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T = ray.mOrigin - tri.A;

	//Calculate u parameter and test bound
	u = glm::dot(T, P) * inv_det;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return false;

	//Prepare to test v parameter
	Q = glm::cross(T, e1);

	//Calculate V parameter and test bound
	v = glm::dot(ray.mDir, Q) * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return false;

	t = glm::dot(e2, Q) * inv_det;

	if (t > EPSILON) { //ray intersection
		hitDist = t;
		return true;
	}

	// No hit, no win
	return false;
}