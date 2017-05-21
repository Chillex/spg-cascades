#include "KDNode.h"
#include "Intersection.h"
#include "Ray.h"

KDNode::KDNode()
{
}

KDNode::~KDNode()
{
}

KDNode* KDNode::Build(std::vector<Triangle*>& triangles, int depth) const
{
	KDNode* node = new KDNode();
	node->triangles = triangles;

	// no triangles, this is a leaf node
	if (triangles.size() == 0)
	{
		node->left = nullptr;
		node->right = nullptr;
		node->boundingBox = new BoundingBox();
		return node;
	}

	// only a single triangle, so give it two empty leaf nodes as children
	if(triangles.size() == 1)
	{
		node->boundingBox = triangles[0]->GetBoundingBox();
		node->left = new KDNode();
		node->left->triangles = std::vector<Triangle*>();
		node->right = new KDNode();
		node->right->triangles = std::vector<Triangle*>();
		return node;
	}

	// get bounding box arround all triangles
	node->boundingBox = triangles[0]->GetBoundingBox();
	
	for (size_t i = 1; i < triangles.size(); ++i)
	{
		node->boundingBox->Expand(triangles[i]->GetBoundingBox());
	}

	// get midpoint of all triangles
	glm::vec3 midpoint(0.0f, 0.0f, 0.0f);
	for (size_t i = 0; i < triangles.size(); ++i)
	{
		midpoint = midpoint + (triangles[i]->GetMidpoint() * (1.0f / triangles.size()));
	}

	// split triangles in left and right
	std::vector<Triangle*> leftTriangles;
	std::vector<Triangle*> rightTriangles;

	// 0 = x, 1 = y, 3 = z
	int axis = node->boundingBox->GetLongestAxis();
	
	// split, based on the triangles midpoints
	for (size_t i = 0; i < triangles.size(); ++i)
	{
		switch (axis)
		{
		case 0:
			midpoint.x <= triangles[i]->GetMidpoint().x ? leftTriangles.push_back(triangles[i]) : rightTriangles.push_back(triangles[i]);
			break;
		case 1:
			midpoint.y <= triangles[i]->GetMidpoint().y ? leftTriangles.push_back(triangles[i]) : rightTriangles.push_back(triangles[i]);
			break;
		case 2:
			midpoint.z <= triangles[i]->GetMidpoint().z ? leftTriangles.push_back(triangles[i]) : rightTriangles.push_back(triangles[i]);
			break;
		}
	}

	// if there are only 10 or less triangles on each side, stop subdiving
	if(leftTriangles.size() <= 10000 && rightTriangles.size() <= 10000)
	{
		node->left = new KDNode();
		node->left->triangles = std::vector<Triangle*>();

		node->right = new KDNode();
		node->right->triangles = std::vector<Triangle*>();
	}
	else
	{
		node->left = Build(leftTriangles, depth + 1);
		node->right = Build(rightTriangles, depth + 1);
	}

	return node;
}

void KDNode::Draw(Shader* shader) const
{
	if(boundingBox != nullptr)
		boundingBox->Draw(shader);

	if (right != nullptr)
		right->Draw(shader);
	
	if (left != nullptr)
		left->Draw(shader);
}

bool KDNode::Hit(const Ray& ray, float t0, float t1, Intersection& intersection) const
{
	// this is an empty node with no bounding box and triangles
	if (boundingBox == nullptr)
		return false;

	// check if the ray intersects with the bounding box of this node
	if (boundingBox->Intersects(ray, t0, t1))
	{
		// highlight the bounding box
		boundingBox->highlight = true;

		// check children
		if (left->triangles.size() > 0 || right->triangles.size() > 0)
		{
			bool hitLeft = left->Hit(ray, t0, t1, intersection);
			bool hitRight = right->Hit(ray, t0, t1, intersection);

			return hitLeft || hitRight;
		}
		else
		{
			bool hitTriangle = false;
			float nearestDistance = t1;

			// this is a leaf node, check intersection with each triangle
			for (size_t i = 0; i < triangles.size(); ++i)
			{
				if(triangles[i]->Intersects(ray, intersection))
				{
					hitTriangle = true;
					if(intersection.distance < nearestDistance)
					{
						nearestDistance = intersection.distance;
						intersection.hitTriangle = triangles[i];
						intersection.point = ray.origin + ray.direction * intersection.distance;
					}
				}
			}

			if(hitTriangle)
			{
				return true;
			}

			return false;
		}

	}
	return false;
}

void KDNode::ClearHighlights()
{
	if (boundingBox != nullptr)
		boundingBox->highlight = false;

	if (right != nullptr)
		right->ClearHighlights();

	if (left != nullptr)
		left->ClearHighlights();
}
