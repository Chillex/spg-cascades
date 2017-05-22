#ifndef RENDER_VOLUME_H
#define RENDER_VOLUME_H

#include <GL/glew.h>
#include "KDTreeNode.h"

class Shader;
class TextureBuffer3D;

class RenderVolume
{
public:
	RenderVolume(GLuint width, GLuint height, GLuint depth);
	~RenderVolume();

	void Render(const TextureBuffer3D& densityTexture, const Shader* shader) const;
	void GenerateGeometry(const TextureBuffer3D& densityTexture, Shader* shader);
	void Render(const Shader* shader) const;

	KDTreeNode* kdRoot;
	std::vector<Triangle> triangles;

private:
	GLuint m_width;
	GLuint m_height;
	GLuint m_depth;

	GLuint m_vao;
	GLuint m_fbo;

	GLuint m_polyCountLutUBO;
	GLuint m_triangeConnectionLutUBO;
	GLuint m_yPositionsUBO;

	GLuint m_geometryVAO;
	GLuint m_geometryTBO;
	GLuint m_query;
	GLuint m_primitivesCount;

	GLuint m_rockTextureX;
	GLuint m_rockTextureY;
	GLuint m_rockTextureZ;

	void BindTextures(GLuint shaderProgram) const;
	void GenerateTextures(void);
	void GenerateTexture(GLuint& textureID, const char* texturePath);
	void CreateKDTree(void);
};

#endif
