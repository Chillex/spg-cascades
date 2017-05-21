#include "ParticleSystem.h"
#include <SOIL/SOIL.h>
#include "Shader.h"

ParticleSystem::ParticleSystem(glm::vec3 emmiterPosition, float emmiterRate)
	: m_emmiterPos(emmiterPosition)
	, m_emmiterRate(emmiterRate)
	, m_maxParticles(1000)
	, m_particleCount(1)
{
	glPointSize(5.0f);
	Particle emmiter = { emmiterPosition, glm::vec3(0.0f, 0.0f, 0.0f), m_emmiterRate, 0 };

	// setup buffer A
	glGenVertexArrays(1, &m_vaoA);
	glGenBuffers(1, &m_vboA);

	glBindVertexArray(m_vaoA);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * m_maxParticles, nullptr, GL_STREAM_COPY);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle), &emmiter);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(Particle), reinterpret_cast<GLvoid*>(7 * sizeof(GLfloat)));

	// setup buffer B
	glGenVertexArrays(1, &m_vaoB);
	glGenBuffers(1, &m_vboB);

	glBindVertexArray(m_vaoB);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * m_maxParticles, nullptr, GL_STREAM_COPY);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<GLvoid*>(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(Particle), reinterpret_cast<GLvoid*>(7 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// generate query
	glGenQueries(1, &m_query);

	// setup input/output
	m_inputVAO = m_vaoA;
	m_outputVAO = m_vaoB;
	m_inputVBO = m_vboA;
	m_outputVBO = m_vboB;
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::UpdateParticles(Shader* shader, GLfloat deltaTime)
{
	shader->Use();

	glUniform1f(glGetUniformLocation(shader->program, "deltaTime"), deltaTime);
	glUniform1f(glGetUniformLocation(shader->program, "emmiterRate"), m_emmiterRate);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_outputVBO);

	glBindVertexArray(m_inputVAO);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_query);
			glBeginTransformFeedback(GL_POINTS);
				glDrawArrays(GL_POINTS, 0, m_particleCount);
			glEndTransformFeedback();
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glBindVertexArray(0);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glDisable(GL_RASTERIZER_DISCARD);

	glFlush();

	glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &m_particleCount);

	// swap buffers
	GLuint tmp = m_inputVBO;
	m_inputVBO = m_outputVBO;
	m_outputVBO = tmp;

	tmp = m_inputVAO;
	m_inputVAO = m_outputVAO;
	m_outputVAO = tmp;
}

void ParticleSystem::Render(Shader* shader) const
{
	glBindVertexArray(m_inputVAO);
		glDrawArrays(GL_POINTS, 0, m_particleCount);
	glBindVertexArray(0);
}
