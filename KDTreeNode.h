#pragma once

#include <vector>

#include "BoundingBox.h"
#include "Triangle.h"

class Shader;

class KDTreeNode
{
public:
	KDTreeNode() 
		: mTriangleIndices()
		, leftChild(nullptr)
		, rightChild(nullptr)
		, bb()
	{}

	void SetBB(const std::vector<Triangle>& triangleStorage);

	void Draw(Shader* shader) const;

	std::vector<uint32_t> mTriangleIndices;
	KDTreeNode* leftChild;
	KDTreeNode* rightChild;
	BoundingBox bb;
};
