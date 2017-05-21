#include "BoundingBox.h"
#include <vector>
#include "Ray.h"

BoundingBox::BoundingBox()
	: BoundingBox(glm::vec3(0.0f), glm::vec3(0.0f))
{
}

BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max)
	: highlight(false)
{
	bounds[0] = min;
	bounds[1] = max;

	// 1x1x1 box
	std::vector<GLfloat> vertices = {
		// front loop
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5,
		0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5,
		// back loop
		-0.5, -0.5,  0.5,
		0.5, -0.5,  0.5,
		0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
		// connections
		-0.5, -0.5, -0.5,
		-0.5, -0.5,  0.5,
		0.5, -0.5, -0.5,
		0.5, -0.5,  0.5,
		0.5,  0.5, -0.5,
		0.5,  0.5,  0.5,
		-0.5,  0.5, -0.5,
		-0.5,  0.5,  0.5,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

BoundingBox::~BoundingBox()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void BoundingBox::Expand(BoundingBox* other)
{
	if (other->bounds[0].x < bounds[0].x) bounds[0].x = other->bounds[0].x;
	if (other->bounds[0].y < bounds[0].y) bounds[0].y = other->bounds[0].y;
	if (other->bounds[0].z < bounds[0].z) bounds[0].z = other->bounds[0].z;

	if (other->bounds[1].x > bounds[1].x) bounds[1].x = other->bounds[1].x;
	if (other->bounds[1].y > bounds[1].y) bounds[1].y = other->bounds[1].y;
	if (other->bounds[1].z > bounds[1].z) bounds[1].z = other->bounds[1].z;
}

// 0 = x, 1 = y, 3 = z
int BoundingBox::GetLongestAxis() const
{
	float lengthX = bounds[1].x - bounds[0].x;
	float lengthY = bounds[1].y - bounds[0].y;
	float lengthZ = bounds[1].z - bounds[0].z;

	if (lengthX > lengthY && lengthX > lengthZ) return 0;
	if (lengthY > lengthZ) return 1;
	return 2;
}

void BoundingBox::Draw(Shader* shader) const
{
	GLint uModel = glGetUniformLocation(shader->program, "model");
	GLint uObjectColor = glGetUniformLocation(shader->program, "objectColor");

	glBindVertexArray(VAO);

	glm::vec3 size = glm::vec3(bounds[1].x - bounds[0].x, bounds[1].y - bounds[0].y, bounds[1].z - bounds[0].z);
	glm::vec3 midpoint = glm::vec3((bounds[0].x + bounds[1].x) / 2.0f, (bounds[0].y + bounds[1].y) / 2.0f, (bounds[0].z + bounds[1].z) / 2.0f);
	glm::mat4 transform = glm::translate(glm::mat4(1), midpoint) * glm::scale(glm::mat4(1), size);

	glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(transform));
	if(!highlight)
		glUniform3f(uObjectColor, 0.0f, 1.0f, 0.0f);
	else
		glUniform3f(uObjectColor, 1.0f, 0.0f, 0.0f);

	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glDrawArrays(GL_LINE_LOOP, 4, 4);
	glDrawArrays(GL_LINES, 8, 8);

	glBindVertexArray(0);
}

// http://www.cs.utah.edu/~awilliam/box/box.pdf
bool BoundingBox::Intersects(const Ray& ray, float t0, float t1)
{
	// start by getting min and max values on the x axis
	float tmin = (bounds[ray.sign.x].x - ray.origin.x) * ray.inverseDirection.x;
	float tmax = (bounds[1 - ray.sign.x].x - ray.origin.x) * ray.inverseDirection.x;

	// get min and max values from the y axis
	float tymin = (bounds[ray.sign.y].y - ray.origin.y) * ray.inverseDirection.y;
	float tymax = (bounds[1 - ray.sign.y].y - ray.origin.y) * ray.inverseDirection.y;

	// if the min of one axis is greater than the max of the other axis, we have no intersection
	if (tmin > tymax || tymin > tmax)
		return false;

	// store the higher min and the lower max value
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	// get the min and max values from the z axis
	float tzmin = (bounds[ray.sign.z].z - ray.origin.z) * ray.inverseDirection.z;
	float tzmax = (bounds[1 - ray.sign.z].z - ray.origin.z) * ray.inverseDirection.z;

	// same as before, if mins are greater than max values => no intersection
	if (tmin > tzmax || tzmin > tmax)
		return false;

	// store higher min and lower max values
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	// now check if the min and max values are within the given interval
	return (tmin < t1) && (tmax > t0);
}
