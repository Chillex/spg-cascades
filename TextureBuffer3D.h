#ifndef TEXTURE_BUFFER_3D_H
#define TEXTURE_BUFFER_3D_H

#include <GL/glew.h>

class TextureBuffer3D
{
public:
	TextureBuffer3D(GLuint width, GLuint height, GLuint depth);
	~TextureBuffer3D();

	void Bind(void) const;
	void Unbind(void) const;

	void BindTexture(void) const;

private:
	GLuint m_fbo;
	GLuint m_texture;

	GLuint m_width;
	GLuint m_height;
	GLuint m_depth;
};

#endif
