#include "Texture2D.h"

#include <SOIL/SOIL.h>

Texture2D::Texture2D(GLchar* path)
{
	// generate texture id and load texture data
	glGenTextures(1, &m_id);
	unsigned char* image = SOIL_load_image(path, &m_width, &m_height, 0, SOIL_LOAD_RGB);

	// assign texture to id
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// unbind and free image
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
}

Texture2D::~Texture2D()
{
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_id);
}
