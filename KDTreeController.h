#pragma once

#include "KDTreeNode.h"
#include "Triangle.h"

class Ray;

class KDTreeController
{
public:
	static KDTreeNode* BuildKDTree(const std::vector<Triangle>& sceneTriangles);
	static bool CheckHit(const Ray& ray, const KDTreeNode* tree, std::vector<std::pair<Triangle, float>>& hitTriangles, std::vector<Triangle>& sceneTriangles);

};
