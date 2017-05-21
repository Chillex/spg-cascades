#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;

class ParticleSystem
{
public:
	ParticleSystem(glm::vec3 emmiterPosition, float emmiterRate);
	~ParticleSystem();

	void UpdateParticles(Shader* shader, GLfloat deltaTime);
	void Render(Shader* shader) const;

private:
	glm::vec3 m_emmiterPos;
	GLfloat m_emmiterRate;
	GLuint m_maxParticles;
	GLuint m_particleCount;

	GLuint m_vaoA;
	GLuint m_vaoB;
	GLuint m_vboA;
	GLuint m_vboB;
	GLuint m_query;

	GLuint m_inputVAO;
	GLuint m_outputVAO;
	GLuint m_inputVBO;
	GLuint m_outputVBO;
};

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	GLfloat lifetime;
	GLint type;
};

#endif
