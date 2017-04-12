#include "TextureBuffer3D.h"
#include <iostream>

TextureBuffer3D::TextureBuffer3D(GLuint width, GLuint height, GLuint depth)
	: m_width(width)
	, m_height(height)
	, m_depth(depth)
{
	// create frame buffer object
	glGenFramebuffers(1, &m_fbo);

	// create 3D texture
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_3D, m_texture);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, m_width, m_height, m_depth, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTextureParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTextureParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// check for errors
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::TEXTURE_BUFFER_3D::FRAMEBUFFER_STATUS_NOT_COMPLETE" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TextureBuffer3D::~TextureBuffer3D()
{
}

void TextureBuffer3D::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_width, m_depth);
}

void TextureBuffer3D::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureBuffer3D::BindTexture() const
{
	glBindTexture(GL_TEXTURE_3D, m_texture);
}
