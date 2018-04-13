#include "SkeletonGenerator.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>


SkeletonGenerator::SkeletonGenerator(SpacialStructure* space)
	: activePoints(0), stepCount(0), spacialStructure(space)
{
}


SkeletonGenerator::~SkeletonGenerator()
{
}

void SkeletonGenerator::begin(std::vector<glm::vec3>& _points, glm::vec3 startingPoint)
{
	clear();

	for (auto& point : _points)
	{
		attractionPoints.emplace_back(point, true);
	}

	activePoints = attractionPoints.size();
	maxDepth = 0;
	trunks = 0;
	addNode(startingPoint, 0);
}

void SkeletonGenerator::step()
{
	if (isFinished())
		return;

	nodeAdded = false;

	const float cullDistance = 0.05f;
	const float searchDistance = 0.1f;
	const float nodeDistance = 0.04f;

	if (attractionPoints.size() < nodes.size());


	// for all the points find the closest node
	for (auto& point : attractionPoints)
	{
		if (!point.second && !trunks)
			continue;

		int closestPoint = -1;
		glm::vec3 difference;
		float distance = FLT_MAX;

		for (int i = 0; i < nodes.size(); i++)
		{
			glm::vec3 diff = point.first - nodes[i].nodePoint;
			float dist = glm::length(diff);

			if (dist < distance && (dist < searchDistance || nodes[i].trunk))
			{
				closestPoint = i;
				difference = diff;
				distance = dist;
			}
		}

		if (closestPoint >= 0 && point.second)
		{
			nodes[closestPoint].nodeWeight += glm::normalize(difference);
		}

		if (distance < cullDistance && closestPoint >= 0)
		{
			if (nodes[closestPoint].trunk) {
				trunks--;
				int pointIndex = closestPoint;
				do
				{
					nodes[pointIndex].trunk = false;
					pointIndex = nodes[pointIndex].nodeParent;
				} while (nodes[pointIndex].trunk || pointIndex);
			}
			if (point.second) activePoints--;
			point.second = false;
		}
	}

	const size_t weightCount = nodes.size();
	for (size_t i = 0; i < weightCount; i++)
	{
		if (nodes[i].nodeWeight != glm::vec3(0.f))
		{
			glm::vec3 newPos(glm::normalize(nodes[i].nodeWeight) * nodeDistance + nodes[i].nodePoint);
			addNode(newPos, i);
			nodes[i].nodeWeight = glm::vec3(0.f);
		}
	}

	stepCount++;
	if (isFinished())
	{
		generateMesh();
	}
}


int SkeletonGenerator::getNodeCount() 
{
	return nodes.size();
}

void SkeletonGenerator::smoothAndUpdate() 
{
	smooth();
	generateMesh();
}

void SkeletonGenerator::smooth()
{
	SkeletonGenerator newNodes = *this;

	for (size_t j = 0; j < nodes[0].nodeChildren.size(); ++j) 
	{
		newNodes.addNode((nodes[0].nodePoint + nodes[nodes[0].nodeChildren[j]].nodePoint) * .5f, 0);
	}

	for (size_t i = 1; i < nodes.size(); ++i)
	{
		int parentIndex = nodes[i].nodeParent;
		newNodes.nodes[i].nodePoint = nodes[parentIndex].nodePoint * .75f + nodes[i].nodePoint * .25f;
		for (size_t j = 0; j < nodes[i].nodeChildren.size(); ++j)
		{
			int childIndex = nodes[i].nodeChildren[j];	
			newNodes.addNode(nodes[i].nodePoint * .75f + nodes[childIndex].nodePoint * .25f, i);

			int parent = newNodes.getNodeCount() - 1;

			for (size_t k = 0; k < nodes[childIndex].nodeChildren.size(); ++k)
			{
				nodes[k].nodeParent = parent;
			}
		}
	}
	*this = newNodes;
}

bool SkeletonGenerator::hasStarted()
{
	return attractionPoints.size() != 0;
}

bool SkeletonGenerator::isFinished()
{
	const size_t maxSteps = -1;
	return hasStarted() && (activePoints <= attractionPoints.size() / 100 || stepCount > maxSteps || !nodeAdded);
}

void SkeletonGenerator::clear()
{
	stepCount = 0;
	attractionPoints.clear();
	nodes.clear();
	nodeIndices.clear();
	nodePositions.clear();
	spacialStructure->clear();
}

std::vector<glm::vec3>& SkeletonGenerator::getNodePositions()
{
	return nodePositions;
}

std::vector<GLuint>& SkeletonGenerator::getNodeIndices()
{
	return nodeIndices;
}

std::vector<glm::vec3>& SkeletonGenerator::getMeshPoints()
{
	return meshPoints;
}

std::vector<glm::vec2>& SkeletonGenerator::getMeshTexCoords()
{
	return meshTexCoords;
}

std::vector<glm::vec3>& SkeletonGenerator::getMeshNormals()
{
	return meshNormals;
}

std::vector<GLuint>& SkeletonGenerator::getMeshIndices()
{
	return meshIndices;
}



void SkeletonGenerator::addNode(glm::vec3 position, size_t parent)
{
	size_t index = nodes.size();
	nodeAdded = true;

	Node newNode;
	newNode.nodePoint = position;
	newNode.nodeWeight = glm::vec3(0);
	newNode.nodeParent = parent;
	newNode.nodeChildren;
	if (parent >= index)
	{
		newNode.depth = 0;
		newNode.trunk = true;
		trunks++;
	}
	else
	{
		nodes[parent].nodeChildren.emplace_back(index);
		newNode.depth = nodes[parent].depth + 1;
		if (nodes[parent].trunk && nodes[parent].nodeChildren.size()) {
			trunks++;
			newNode.trunk = true;
		}
		else
		{
			newNode.trunk = nodes[parent].trunk;
		}
	}


	if (newNode.depth > maxDepth) maxDepth = newNode.depth;

	nodes.emplace_back(newNode);

	nodeIndices.emplace_back(parent);
	nodeIndices.emplace_back(index);

	nodePositions.emplace_back(position);

	spacialStructure->addTreeNode(&position, (int)index);
}

void SkeletonGenerator::generateMesh()
{
	meshPoints.clear();
	meshTexCoords.clear();
	meshNormals.clear();
	meshIndices.clear();
	for (size_t i = 0; i < nodes.size(); i++)
	{
		createRevolution(i, nodes[i].nodeParent);
	}
}

void SkeletonGenerator::createRevolution(size_t point1, size_t point2)
{
	if (point1 == point2)
		return;

	float nodeDepth1 = (float)nodes[point1].depth;
    float nodeDepth2 = (float)nodes[point2].depth;

	glm::vec3 diff = glm::normalize(nodes[point1].nodePoint - nodes[point2].nodePoint);
	glm::vec3 out = glm::normalize(glm::cross(diff, glm::vec3(diff.y, diff.x, diff.z)));

	size_t indexOffset = 0;
	if (meshPoints.size())
		indexOffset = meshPoints.size() - 1;

	const size_t u_steps = 3;
	const size_t v_steps = 4;

	for (size_t ui = 0; ui < u_steps; ui++)
	{
		const float u = ((float)ui) / (u_steps - 1);

        volatile float nodeDepth = (1 - u)*nodeDepth1 + u*nodeDepth2;

		glm::vec3 linePoint = (1.f - u) * nodes[point1].nodePoint + u * nodes[point2].nodePoint;

		for (size_t vi = 0; vi < v_steps; vi++)
		{
			float v = (((float)vi) / (v_steps - 1)) * 2.f * (float)M_PI;
			if (vi + 1 == v_steps)
				v = 0.f;

			glm::vec3 normal = glm::vec3(glm::vec4(out, 1.f) * glm::rotate(glm::mat4(1.f), v, diff));

 
			normal *= std::min(0.01f, 0.03f / nodeDepth);

			meshPoints.emplace_back(linePoint + normal);
			meshTexCoords.emplace_back(u, ((float)vi) / (v_steps - 1));
			meshNormals.emplace_back(normal);

			if (ui + 1 < u_steps && vi + 1 < v_steps)
			{
				meshIndices.emplace_back(ui*v_steps + vi + indexOffset);
				meshIndices.emplace_back(ui*v_steps + vi + 1 + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + indexOffset);

				meshIndices.emplace_back(ui*v_steps + vi + 1 + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + 1 + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + indexOffset);
			}
		}
	}

}
