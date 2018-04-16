#include "SkeletonGenerator.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>


SkeletonGenerator::SkeletonGenerator(SpacialStructure* space)
	: activePoints(0), stepCount(0), spacialStructure(space)
	, cullDistance(0.05f), searchDistance(0.1f), nodeDistance(0.04f)
	, minRadius(0.0f), maxRadius(0.1f)
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
	finish = false;
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
			bool nearNode = false;
			Node checkNode = nodes[i];

			for (size_t i = 0; i <= 3 && !nearNode; ++i)
			{
				if (length(checkNode.nodePoint - newPos) < cullDistance / 2.f) nearNode = true;
				checkNode = nodes[checkNode.nodeParent];
			}

			if (!nearNode)
			{
				addNode(newPos, i);
			}

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

void SkeletonGenerator::setCullDistance(float d)
{
	cullDistance = d;
}

void SkeletonGenerator::setSearchDistance(float d)
{
	searchDistance = d;
}

void SkeletonGenerator::setNodeDistance(float d)
{
	nodeDistance = d;
}

void SkeletonGenerator::setMaxRadius(float d)
{
	bool changed = false;
	if (d != maxRadius)
		changed = true;

	maxRadius = d;

	if (changed)
		generateMesh();
}

void SkeletonGenerator::setMinRadius(float d)
{
	bool changed = false;
	if (d != maxRadius)
		changed = true;

	minRadius = d;

	if (changed)
		generateMesh();
}

float SkeletonGenerator::getDepth(size_t i) 
{
	if (nodes[i].depth) return nodes[i].depth;
	
	if (!nodes[i].nodeChildren.size()) 
	{
		nodes[i].depth = 1;
		return nodes[i].depth;
	}

	size_t biggestChild = 0;
	for (size_t j = 0; j < nodes[i].nodeChildren.size(); ++j)
	{
		if (nodes[i].depth < getDepth(nodes[i].nodeChildren[j]))
		{
			biggestChild = j;
			nodes[i].depth = getDepth(nodes[i].nodeChildren[j]);
		}
	}

	nodes[i].depth++;

	return nodes[i].depth;
}

void SkeletonGenerator::calculateDepths() 
{
	for (auto& node : nodes)
		node.depth = 0;

	maxDepth = 100;
	minDepth = maxDepth / getDepth(0);;

	for (auto& node : nodes)
		node.depth *= minDepth;
}

void SkeletonGenerator::calculateSizes() 
{
	for (auto& node : nodes)
		node.size = 0;

	calculateDepths();
}

void SkeletonGenerator::smooth()
{
	SkeletonGenerator newNodes = *this;

	newNodes.nodes[0].nodeChildren.clear();
	for (size_t j = 0; j < nodes[0].nodeChildren.size(); ++j) 
	{
		newNodes.addNode((nodes[0].nodePoint + nodes[nodes[0].nodeChildren[j]].nodePoint) * .5f, 0);
		int newNodeIndex = newNodes.getNodeCount() - 1;
		int childIndex = nodes[0].nodeChildren[j];
		newNodes.nodes[newNodeIndex].nodeChildren.emplace_back(childIndex);
	}

	for (size_t i = 1; i < nodes.size(); ++i)
	{
		newNodes.nodes[i].nodeChildren.clear();
		if (nodes[i].nodeChildren.size())
		{
			newNodes.nodes[i].nodePoint = nodes[nodes[i].nodeParent].nodePoint * .25f + nodes[i].nodePoint * .75f;
			newNodes.nodePositions[i] = newNodes.nodes[i].nodePoint;

			for (size_t j = 0; j < nodes[i].nodeChildren.size(); ++j)
			{
				int childIndex = nodes[i].nodeChildren[j];
				newNodes.addNode(nodes[i].nodePoint * .75f + nodes[childIndex].nodePoint * .25f, i);

				int newNodeIndex = newNodes.getNodeCount() - 1;
				newNodes.nodes[newNodeIndex].nodeChildren.emplace_back(childIndex);
			}
		}
		else 
		{
			newNodes.nodes[i].nodePoint = nodes[nodes[i].nodeParent].nodePoint * .5f + nodes[i].nodePoint * .5f;
			newNodes.nodePositions[i] = newNodes.nodes[i].nodePoint;

			newNodes.addNode(nodes[i].nodePoint, i);
		}
	}

	for (size_t i = 0; i < newNodes.nodes.size(); ++i)
	{
		newNodes.nodes[i].depth = 0.f;
		for (size_t j = 0; j < newNodes.nodes[i].nodeChildren.size(); ++j)
		{
			newNodes.nodes[newNodes.nodes[i].nodeChildren[j]].nodeParent = i;

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
	return hasStarted() && finish;
}

void SkeletonGenerator::forceFinished()
{
	finish = true;
}

void SkeletonGenerator::clear()
{
	stepCount = 0;
	finish = false;
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
		newNode.trunk = true;
		trunks++;
	}
	else
	{
		nodes[parent].nodeChildren.emplace_back(index);
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
	if (nodes.size() == 0)
		return;

	calculateSizes();

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

	float nodeDepth1 = ((float)getDepth(point1) - minDepth) / maxDepth;
	float nodeDepth2 = ((float)getDepth(point2) - minDepth) / maxDepth;

	glm::vec3 diff1 = glm::normalize(nodes[point1].nodePoint - nodes[point2].nodePoint);
	glm::vec3 out1 = glm::normalize(glm::cross(diff1, glm::vec3(diff1.y, diff1.x, diff1.z)));

	
	
	glm::vec3 diff2 = glm::normalize(nodes[point2].nodePoint - nodes[nodes[point2].nodeParent].nodePoint);	
	if (nodes[point2].nodePoint == nodes[nodes[point2].nodeParent].nodePoint)
		diff2 = diff1;
	
	glm::vec3 out2 = glm::normalize(glm::cross(diff2, glm::vec3(diff2.y, diff2.x, diff2.z)));


	size_t indexOffset = 0;
	if (meshPoints.size())
		indexOffset = meshPoints.size();


	const size_t u_steps = 6;
	const size_t v_steps = 6;

	for (size_t ui = 0; ui < u_steps; ui++)
	{
		const float u = ((float)ui) / (u_steps - 1);

		float nodeDepth = std::max(minRadius, maxRadius * ((1 - u)*nodeDepth1 + u * nodeDepth2));

		glm::vec3 linePoint = (1.f - u) * nodes[point1].nodePoint + u * nodes[point2].nodePoint;
		glm::vec3 out = glm::normalize((1.f - u) * out1 + u * out2);
		glm::vec3 diff = glm::normalize((1.f - u) * diff1 + u * diff2);

		for (size_t vi = 0; vi < v_steps; vi++)
		{
			float v = (((float)vi) / (v_steps - 1)) * 2.f * (float)M_PI;
			if (vi + 1 == v_steps)
				v = 0.f;

			glm::vec3 normal = glm::vec3(glm::vec4(out, 1.f) * glm::rotate(glm::mat4(1.f), v, diff));

			normal *= nodeDepth;

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
