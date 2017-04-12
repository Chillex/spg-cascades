#ifndef TEXTURE_BUFFER_H
#define TEXTURE_BUFFER_H
#include <GL/glew.h>

class TextureBuffer
{
public:
	TextureBuffer(GLuint width, GLuint height);
	~TextureBuffer();

	void Bind(void) const;
	void Unbind(void) const;

	void BindTexture(void) const;

private:
	GLuint m_fbo;
	GLuint m_texture;

	GLuint m_width;
	GLuint m_height;
};

#endif
