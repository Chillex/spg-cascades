#include "TextureBuffer.h"

#include <iostream>

TextureBuffer::TextureBuffer(GLuint width, GLuint height)
	: m_width(width)
	, m_height(height)
{
	// create frame buffer object
	glGenFramebuffers(1, &m_fbo);

	// create texture
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// attach texture to frame buffer object
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::TEXTURE_BUFFER::FRAMEBUFFER_STATUS_NOT_COMPLETE" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TextureBuffer::~TextureBuffer()
{
}

void TextureBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, m_width, m_height);
}

void TextureBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TextureBuffer::BindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}
