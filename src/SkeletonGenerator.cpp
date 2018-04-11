#include "SkeletonGenerator.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>


SkeletonGenerator::SkeletonGenerator()
	: activePoints(0), stepCount(0)
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
	activePoints = points.size();
	addNode(startingPoint, 0);
}

void SkeletonGenerator::step()
{
	if (isFinished())
		return;

	const float cullDistance = 0.051f;
	const float nodeDistance = 0.025f;

	for (auto& weight : nodes.nodeWeights)
	{
		weight = glm::vec3(0.f);
	}

	for (auto& point : points)
	{
		if (!point.second)
			continue;

		size_t closestPoint = -1;
		glm::vec3 difference;
		float distance = FLT_MAX;
		
		for (size_t i = 0; i < nodes.nodePoints.size(); i++)
		{
			glm::vec3 diff = point.first - nodes.nodePoints[i];
			float dist = glm::length(diff);

			if (dist < distance)
			{
				closestPoint = i;
				difference = diff;
				distance = dist;
			}
		}

		nodes.nodeWeights[closestPoint] += glm::normalize(difference);


		if (distance < cullDistance)
		{
			point.second = false;
			activePoints--;
		}
	}

	const size_t weightCount = nodes.nodeWeights.size();
	for (size_t i = 0; i < weightCount; i++)
	{
		if (nodes.nodeWeights[i] != glm::vec3(0.f))
		{
			glm::vec3 newPos(glm::normalize(nodes.nodeWeights[i]) * nodeDistance + nodes.nodePoints[i]);
			addNode(newPos, i);
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
	const size_t maxSteps = 110;
	return hasStarted() && (activePoints <= points.size() / 100 || stepCount > maxSteps);
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
	if (parent < nodes.nodeChildren.size())
		nodes.nodeChildren[parent].emplace_back(nodes.nodeChildren.size());

	nodes.nodePoints.emplace_back(position);
	nodes.nodeWeights.emplace_back(glm::vec3(0.f));
	nodes.nodeParents.emplace_back(parent);
	nodes.nodeChildren.emplace_back();
	nodeIndices.emplace_back(parent);
	nodeIndices.emplace_back(nodes.nodePoints.size() - 1);
}

void SkeletonGenerator::generateMesh()
{
	meshPoints.clear();
	meshTexCoords.clear();
	meshNormals.clear();
	meshIndices.clear();


	auto list = makeRevolutionList(nodes);

	for (auto& nodePair : list)
	{
		createRevolution(nodes, nodePair.first, nodePair.second);
	}
}

std::vector<std::pair<size_t,size_t>> SkeletonGenerator::makeRevolutionList(NodeList& nodes, size_t start)
{
	std::vector<std::pair<size_t, size_t>> nodeList;

	if (!nodes.nodeChildren[start].size())
		return nodeList;

	size_t nextNode = start;
	while (nodes.nodeChildren[nextNode].size() == 1)
	{
		nextNode = nodes.nodeChildren[nextNode].front();
	}

	nodeList.emplace_back(start, nextNode);

	for (auto& node : nodes.nodeChildren[nextNode])
	{
		nodeList.emplace_back(nextNode, node);
		auto newList = makeRevolutionList(nodes, node);
		nodeList.insert(nodeList.end(), newList.begin(), newList.end());
	}

	return nodeList;
}

void SkeletonGenerator::createRevolution(NodeList& nodes, size_t point1, size_t point2)
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
	glm::vec3 out = glm::normalize(glm::cross(diff,glm::vec3(diff.y,diff.x,diff.z)));

	size_t indexOffset = 0;
	if (meshPoints.size())
	 indexOffset = meshPoints.size() - 1;

	const size_t u_steps = 4;
	const size_t v_steps = 4;

	for (size_t ui = 0; ui < u_steps; ui++)
	{
		const float u = ((float)ui) / (u_steps - 1);
		glm::vec3 linePoint = (1.f - u) * nodes.nodePoints[point1] + u * nodes.nodePoints[point2];

		for (size_t vi = 0; vi < v_steps; vi++)
		{
			float v = ( ((float)vi) / v_steps) * 2 * M_PI;

			glm::vec3 normal = glm::vec3(glm::vec4(out,1.f) * glm::rotate(glm::mat4(1.f), v, diff));

			normal *= std::min(0.01f, (0.03f / nodeDepth)*(1.f-u) + (0.03f / (nodeDepth+1))*u);

			meshPoints.emplace_back(linePoint + normal);
			meshTexCoords.emplace_back(u, ((float)vi) / (v_steps - 1));
			meshNormals.emplace_back(-normal);

			if (ui + 1 < u_steps && vi + 1 < v_steps)
			{
				meshIndices.emplace_back(ui*v_steps + vi + indexOffset);
				meshIndices.emplace_back(ui*v_steps + vi + 1 + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + indexOffset);

				meshIndices.emplace_back(ui*v_steps + vi + 1 + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + 1 + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + indexOffset);
			}
			else if (ui + 1 < u_steps)
			{
				meshIndices.emplace_back(ui*v_steps + vi + indexOffset);
				meshIndices.emplace_back(ui*v_steps + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + indexOffset);

				meshIndices.emplace_back(ui*v_steps + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + indexOffset);
				meshIndices.emplace_back((ui + 1)*v_steps + vi + indexOffset);
			}


		}

	}

}
