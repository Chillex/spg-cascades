#ifndef KD_NODE_H
#define KD_NODE_H

#include <vector>
#include "Triangle.h"

struct Intersection;

class KDNode
{
public:
	KDNode();
	~KDNode();

	KDNode* Build(std::vector<Triangle*>& triangles, int depth) const;
	void Draw(Shader* shader) const;
	bool Hit(const Ray& ray, float t0, float t1, Intersection& intersection) const;
	void ClearHighlights();
	
	BoundingBox* boundingBox;
	KDNode* left;
	KDNode* right;
	std::vector<Triangle*> triangles;
};

#endif