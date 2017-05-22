#ifndef _RAY_H_
#define _RAY_H_
#include <glm/detail/type_vec3.hpp>

class Ray
{
public:
	Ray(glm::vec3 origin, glm::vec3 direction);

	glm::vec3 mOrigin;
	glm::vec3 mDir;
	glm::vec3 mInvDir;
	int mSign[3];
};

#endif // _RAY_H_