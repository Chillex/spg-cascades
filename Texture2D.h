#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <GL/glew.h>

class Texture2D
{
public:
	explicit Texture2D(GLchar* path);
	~Texture2D();

	void Bind(void) const;

private:
	GLuint m_id;
	GLint m_width;
	GLint m_height;
};

#endif
