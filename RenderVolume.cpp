#include "RenderVolume.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "TextureBuffer3D.h"
#include "Shader.h"
#include "LUT.h"
#include <SOIL/SOIL.h>

RenderVolume::RenderVolume(GLuint width, GLuint height, GLuint depth)
	: m_width(width)
	, m_height(height)
	, m_depth(depth)
{
	// generate dummy vertex buffer with one entry for each voxel in a layer
	// (50x50 size = 49 voxels on each layer)
	std::vector<GLfloat> voxels;
	voxels.reserve(m_width * m_depth * 2); // two values per voxel

	for(GLuint x = 0; x < m_width; ++x)
	{
		for(GLuint z = 0; z < m_depth; ++z)
		{
			GLfloat u = static_cast<GLfloat>(x) / static_cast<GLfloat>(m_width);
			GLfloat v = static_cast<GLfloat>(z) / static_cast<GLfloat>(m_depth);

			voxels.emplace_back(u);
			voxels.emplace_back(v);
		}
	}

	// create the VAO and VBO
	glGenBuffers(1, &m_fbo);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_fbo);
			glBufferData(GL_ARRAY_BUFFER, voxels.size() * sizeof(GLfloat), &voxels[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create look up table UBOs
	glGenBuffers(1, &m_polyCountLutUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_polyCountLutUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(GLuint) * 256, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLuint) * 256, &LUT::polyCountTable);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &m_triangeConnectionLutUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_triangeConnectionLutUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(GLint) * 256 * 16, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLint) * 256 * 16, &LUT::triangleConnectionTable);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	GLfloat* yPosition = new GLfloat[m_height];
	for (int layer = 0; layer < m_height; ++layer)
	{
		yPosition[layer] = layer * (1.0f / static_cast<GLfloat>(m_height));
	}
	glGenBuffers(1, &m_yPositionsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_yPositionsUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat) * height, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat) * height, yPosition);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	GenerateTextures();
}

RenderVolume::~RenderVolume()
{
}

void RenderVolume::Render(const TextureBuffer3D& densityTexture, const Shader* shader) const
{
	// shader uniforms
	glUniform1f(glGetUniformLocation(shader->program, "worldSpaceVolumeHeight"), 1.0f * (static_cast<GLfloat>(m_height) / static_cast<GLfloat>(m_width)));
	glUniform1f(glGetUniformLocation(shader->program, "worldSpaceVoxelSize"), 1.0f / static_cast<GLfloat>(m_width));
	glUniform3f(glGetUniformLocation(shader->program, "invertedVoxelDimension"), 1.0f / static_cast<GLfloat>(m_width - 1), 1.0f / static_cast<GLfloat>(m_height - 1), 1.0f / static_cast<GLfloat>(m_depth - 1));

	// LUTs
	//glUniformBlockBinding(shader->program, glGetUniformBlockIndex(shader->program, "lut_poly"), 0);
	//glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_polyCountLutUBO);

	GLuint triLutIndex = glGetUniformBlockIndex(shader->program, "triangleConnectionLUT");
	glUniformBlockBinding(shader->program, triLutIndex, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_triangeConnectionLutUBO);

	GLuint yPosIndex = glGetUniformBlockIndex(shader->program, "yPosition");
	glUniformBlockBinding(shader->program, yPosIndex, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_yPositionsUBO);

	glActiveTexture(GL_TEXTURE0);
	densityTexture.BindTexture();
	glUniform1i(glGetUniformLocation(shader->program, "densityTexture"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_rockTextureX);
		glUniform1i(glGetUniformLocation(shader->program, "rockTextureX"), 1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_rockTextureY);
			glUniform1i(glGetUniformLocation(shader->program, "rockTextureY"), 2);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, m_rockTextureZ);
				glUniform1i(glGetUniformLocation(shader->program, "rockTextureZ"), 3);
					glBindVertexArray(m_vao);
						glDrawArraysInstanced(GL_POINTS, 0, m_width * m_depth * 2, m_height - 1);
					glBindVertexArray(0);
}

void RenderVolume::GenerateTextures()
{
	GenerateTexture(m_rockTextureX, "Assets/Textures/rock.jpg");
	GenerateTexture(m_rockTextureY, "Assets/Textures/moss.jpg");
	GenerateTexture(m_rockTextureZ, "Assets/Textures/rock2.jpg");
}

void RenderVolume::GenerateTexture(GLuint& textureID, const char* texturePath)
{
	int width;
	int height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, nullptr, SOIL_LOAD_RGB);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}
