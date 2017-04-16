#ifndef RENDER_VOLUME_H
#define RENDER_VOLUME_H

#include <GL/glew.h>

class Shader;
class TextureBuffer3D;

class RenderVolume
{
public:
	RenderVolume(GLuint width, GLuint height, GLuint depth);
	~RenderVolume();

	void Render(const TextureBuffer3D& densityTexture, const Shader* shader) const;

private:
	GLuint m_width;
	GLuint m_height;
	GLuint m_depth;

	GLuint m_vao;
	GLuint m_fbo;

	GLuint m_polyCountLutUBO;
	GLuint m_triangeConnectionLutUBO;
	GLuint m_yPositionsUBO;
};

#endif
