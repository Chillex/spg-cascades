#include "KDTreeNode.h"
#include <ostream>
#include <iostream>

void KDTreeNode::SetBB(const std::vector<Triangle>& triangleStorage)
{
	for (unsigned int i = 0; i < mTriangleIndices.size(); ++i)
	{
		if (i == 0)
		{
			bb.mMin = bb.mMax = triangleStorage[mTriangleIndices[i]].A;
		}

		for (int j = 0; j < 3; ++j)
		{
			glm::vec3 currPt;

			if (j == 0)
				currPt = triangleStorage[mTriangleIndices[i]].A;
			else if (j == 1)
				currPt = triangleStorage[mTriangleIndices[i]].B;
			else
				currPt = triangleStorage[mTriangleIndices[i]].C;

			if (currPt.x > bb.mMax.x) bb.mMax.x = currPt.x;
			if (currPt.y > bb.mMax.y) bb.mMax.y = currPt.y;
			if (currPt.z > bb.mMax.z) bb.mMax.z = currPt.z;
			if (currPt.x < bb.mMin.x) bb.mMin.x = currPt.x;
			if (currPt.y < bb.mMin.y) bb.mMin.y = currPt.y;
			if (currPt.z < bb.mMin.z) bb.mMin.z = currPt.z;
		}
	}
}

void KDTreeNode::Draw(Shader* shader) const
{
	bb.Draw(shader);

	if (rightChild != nullptr)
		rightChild->Draw(shader);

	if (leftChild != nullptr)
		leftChild->Draw(shader);
}
