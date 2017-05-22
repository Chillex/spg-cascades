#include "Ray.h"

Ray::Ray(glm::vec3 origin, glm::vec3 direction)
	: mOrigin(origin)
	, mDir(direction)
	, mInvDir(1.0f / mDir)
{
	mSign[0] = (mInvDir.x < 0.0f);
	mSign[1] = (mInvDir.y < 0.0f);
	mSign[2] = (mInvDir.z < 0.0f);
}
