#include "BoundingBox.h"

#include "Shader.h"
#include <glm/gtc/matrix_transform.inl>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

BoundingBox::BoundingBox()
	: mMin(0, 0, 0)
	, mMax(0, 0, 0)
	, highlight(false)
{
	CreateVAO();
}

BoundingBox::BoundingBox(glm::vec3 minP, glm::vec3 maxP)
	: mMin(minP)
	, mMax(maxP)
	, highlight(false)
{
	CreateVAO();
}

uint32_t BoundingBox::GetLongestAxis(void) const
{
	float xLength = mMax.x - mMin.x;
	float yLength = mMax.y - mMin.y;
	float zLength = mMax.z - mMin.z;

	if (yLength > xLength)
		return 1;

	if (zLength > yLength && zLength > xLength)
		return 2;

	return 0;
}

void BoundingBox::Draw(Shader* shader) const
{
	GLuint uModel = glGetUniformLocation(shader->program, "model");
	GLuint uObjectColor = glGetUniformLocation(shader->program, "objectColor");

	glBindVertexArray(m_vao);

	glm::vec3 size = glm::vec3(mMax.x - mMin.x, mMax.y - mMin.y, mMax.z - mMin.z);
	glm::vec3 midpoint = glm::vec3((mMin.x + mMax.x) / 2.0f, (mMin.y + mMax.y) / 2.0f, (mMin.z + mMax.z) / 2.0f);
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), midpoint) * glm::scale(glm::mat4(1.0f), size);

	glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(transform));
	if (!highlight)
		glUniform3f(uObjectColor, 0.0f, 1.0f, 0.0f);
	else
		glUniform3f(uObjectColor, 1.0f, 0.0f, 0.0f);

	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glDrawArrays(GL_LINE_LOOP, 4, 4);
	glDrawArrays(GL_LINES, 8, 8);

	glBindVertexArray(0);
}

void BoundingBox::CreateVAO(void)
{
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

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
