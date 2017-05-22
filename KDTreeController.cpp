#include "KDTreeController.h"
#include <stack>
#include <numeric>
#include <ostream>
#include <iostream>
#include <algorithm>
#include "Ray.h"
#include "MathUtil.h"

static const uint32_t TRIANGLE_TRESHHOLD = 100;

KDTreeNode* KDTreeController::BuildKDTree(const std::vector<Triangle>& sceneTriangles)
{
	bool notFinished = true;

	std::stack<KDTreeNode*> openList;
	
	KDTreeNode* root = new KDTreeNode;
	root->mTriangleIndices.resize(sceneTriangles.size());
	std::iota(root->mTriangleIndices.begin(), root->mTriangleIndices.end(), 0);

	openList.push(root);

	glm::vec3 medianPt;

	while (!openList.empty())
	{
		/// Split the node
		KDTreeNode* current = openList.top();
		openList.pop();

		/// BB for current Node
		current->SetBB(sceneTriangles);

		/// Check if we are a leaf
		if (current->mTriangleIndices.size() < 100)
		{
			continue;
		}

		uint32_t axis = current->bb.GetLongestAxis();

		/// axis == 0 -> x-axis
		/// axis == 1 -> y-axis
		/// axis == 2 -> z-axis
		std::nth_element(current->mTriangleIndices.begin(), current->mTriangleIndices.begin() + current->mTriangleIndices.size() / 2, current->mTriangleIndices.end(), [&axis, &sceneTriangles](uint32_t tri1Index, uint32_t tri2Index)
		{
			return sceneTriangles[tri1Index].GetCentroid()[axis] < sceneTriangles[tri2Index].GetCentroid()[axis];
		});

		medianPt = sceneTriangles[current->mTriangleIndices[current->mTriangleIndices.size() / 2]].GetCentroid();
		current->leftChild = new KDTreeNode;
		current->rightChild = new KDTreeNode;

		/// Iterator over all triangle indices of current node
		for (uint32_t index : current->mTriangleIndices)
		{
			medianPt[axis] >= sceneTriangles[index].GetCentroid()[axis] ?
				current->leftChild->mTriangleIndices.push_back(index) : current->rightChild->mTriangleIndices.push_back(index);
		}

		/// Push tha children onto tha stack
		openList.push(current->rightChild);
		openList.push(current->leftChild);
	}

	return root;
}

bool KDTreeController::CheckHit(const Ray& ray, const KDTreeNode* tree, std::vector<std::pair<Triangle, float>>& hitTriangles, std::vector<Triangle>& sceneTriangles)
{
	bool hitBB = RayBoxIntersection(ray, tree->bb);

	if (hitBB)
	{
		if (tree->leftChild == nullptr && tree->rightChild == nullptr)
		{
			/// We hit a leaf, check all triangles
			// std::cout << "I have " << node->nodeTriangles.size() << " triangles to test!" << std::endl

			for (uint32_t tri : tree->mTriangleIndices)
			{
				float hitDist = 0.0f;
				bool hitTri = RayTriangleIntersection(ray, sceneTriangles[tri], hitDist);

				if (hitTri)
					hitTriangles.push_back(std::make_pair(sceneTriangles[tri], hitDist));
			}
		}
		else
		{
			if (tree->leftChild != nullptr)
				CheckHit(ray, tree->leftChild, hitTriangles, sceneTriangles);
			if (tree->rightChild != nullptr)
				CheckHit(ray, tree->rightChild, hitTriangles, sceneTriangles);

			return false;
		}
	}

	return true;
};
