#include "SkeletonGenerator.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


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
		points.emplace_back(point, true);
	}

	trunk = true;
	activePoints = points.size();
	addNode(startingPoint, 0);
}

void SkeletonGenerator::step()
{
	if (isFinished())
		return;

	nodeAdded = false;

	const float cullDistance = 0.005f;
	const float searchDistance = 0.1f;
	const float nodeDistance = 0.004f;

	// for all the points find the closest node
	for (auto& point : points)
	{
		if (!point.second)
			continue;

		int closestPoint = -1;
		glm::vec3 difference;
		float distance = FLT_MAX;

		std::vector<int> nearNodes;

		glm::vec3 spacing = spacialStructure->getSpacing();
		glm::vec3 spacialSearchRadius = (searchDistance * 2.f / spacing) + spacing;


		//std::cout << nodes.nodePoints.size();
		int nodeNum = 0;
		for (float x = 0; x <= 1; x += spacing.x)
		{
			for (float y = 0; y <= 1; y += spacing.y)
			{
				for (float z = 0; z <= 1; z += spacing.z)
				{
					nodeNum += spacialStructure->getTreeNodes(glm::vec3(x, y, z)).size();
				}
			}
		}

		//std::cout << " ==? " << nodeNum << std::endl;

		if (!trunk && 0)
		{
			//iterate over the search space
			for (float x = point.first.x - spacialSearchRadius.x; x <= point.first.x + spacialSearchRadius.x; x += spacing.x)
			{
				for (float y = point.first.y - spacialSearchRadius.y; y <= point.first.y + spacialSearchRadius.y; y += spacing.y)
				{
					for (float z = point.first.z - spacialSearchRadius.z; z <= point.first.z + spacialSearchRadius.z; z += spacing.z)
					{
						std::vector<int> cellNodes = spacialStructure->getTreeNodes(glm::vec3(x, y, z));
						nearNodes.insert(nearNodes.end(), cellNodes.begin(), cellNodes.end());
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < nodes.nodePoints.size(); ++i)
			{
				nearNodes.emplace_back(i);
			}
		}
		// check for the closest tree node to the point
		for (int i : nearNodes)
		{
			glm::vec3 diff = point.first - nodes.nodePoints[i];
			float dist = glm::length(diff);

			if (dist < distance && (dist < searchDistance || trunk))
			{
				closestPoint = i;
				difference = diff;
				distance = dist;
			}
		}

		if (closestPoint >= 0)
		{
			nodes.nodeWeights[closestPoint] += glm::normalize(difference);
		}
		if (distance < cullDistance)
		{
			trunk = false;
			point.second = false;
			activePoints--;
		}
	}

	const size_t weightCount = nodes.nodeWeights.size();
	for (size_t i = 0; i < weightCount; i++)
	{
		if (nodes.nodeWeights[i].x > 0.0001f || nodes.nodeWeights[i].y > 0.0001f || nodes.nodeWeights[i].z > 0.0001f)
		{
			glm::vec3 newPos(glm::normalize(nodes.nodeWeights[i]) * nodeDistance + nodes.nodePoints[i]);
			addNode(newPos, i);
			nodes.nodeWeights[i] = glm::vec3(0.f);
		}
	}

	stepCount++;
	if (isFinished())
	{
		generateMesh();
	}
}

bool SkeletonGenerator::hasStarted()
{
	return points.size() != 0;
}

bool SkeletonGenerator::isFinished()
{
	const size_t maxSteps = -1;
	return hasStarted() && (activePoints <= points.size() / 100 || stepCount > maxSteps || !nodeAdded);
}

void SkeletonGenerator::clear()
{
	stepCount = 0;
	points.clear();
	nodes.nodePoints.clear();
	nodes.nodeWeights.clear();
	nodes.nodeParents.clear();
	nodes.nodeChildren.clear();
	nodeIndices.clear();

	spacialStructure->clear();
}

std::vector<glm::vec3>& SkeletonGenerator::getNodePositions()
{
	return nodes.nodePoints;
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
	nodeAdded = true;

	std::cout << position.x  << " " << position.y << " "<< position.z << std::endl;

	int index = nodes.nodePoints.size();

	if (parent < nodes.nodeChildren.size())
	{
		nodes.nodeChildren[parent].emplace_back(nodes.nodeChildren.size());
	}

	nodes.nodePoints.emplace_back(position);
	nodes.nodeWeights.emplace_back(glm::vec3(0.f));
	nodes.nodeParents.emplace_back(parent);
	nodes.nodeChildren.emplace_back();
	nodeIndices.emplace_back(parent);
	nodeIndices.emplace_back(index);

	spacialStructure->addTreeNode(&position, index);
}

void SkeletonGenerator::generateMesh()
{
	meshPoints.clear();
	meshTexCoords.clear();
	meshNormals.clear();
	meshIndices.clear();
	for (size_t i = 0; i < nodes.nodeParents.size(); i++)
	{
		createRevolution(i, nodes.nodeParents[i]);
	}
}

void SkeletonGenerator::createRevolution(size_t point1, size_t point2)
{
	if (point1 == point2)
		return;


	size_t nodeDepth = 0;
	{
		size_t parent = nodes.nodeParents[point1];
		while (parent != 0)
		{
			parent = nodes.nodeParents[parent];
			nodeDepth++;
		}
	}

	
	glm::vec3 diff = glm::normalize(nodes.nodePoints[point1] - nodes.nodePoints[point2]);
	glm::vec3 out = glm::normalize(glm::cross(diff, glm::vec3(diff.y, diff.x, diff.z)));

	size_t indexOffset = 0;
	if (meshPoints.size())
		indexOffset = meshPoints.size() - 1;

	const size_t u_steps = 3;
	const size_t v_steps = 4;

	for (size_t ui = 0; ui < u_steps; ui++)
	{
		const float u = ((float)ui) / (u_steps - 1);
		glm::vec3 linePoint = (1.f - u) * nodes.nodePoints[point1] + u * nodes.nodePoints[point2];

		for (size_t vi = 0; vi < v_steps; vi++)
		{
			float v = (((float)vi) / (v_steps - 1)) * 2 * M_PI;
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
