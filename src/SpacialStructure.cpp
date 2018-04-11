#include "SpacialStructure.h"

SpacialStructure::SpacialStructure() : SpacialStructure(glm::vec3(100.f))
{
}

SpacialStructure::SpacialStructure(glm::vec3 dimensions): SpacialStructure(dimensions, glm::vec3(-1), glm::vec3(1))
{
}

SpacialStructure::SpacialStructure(glm::vec3 dimensions, glm::vec3 _mins, glm::vec3 _maxs)
{
	mins = _mins;
	maxs = _maxs;

	spacialX = floor(dimensions.x);
	spacialY = floor(dimensions.y);
	spacialZ = floor(dimensions.z);

	spacialStructure = new cell**[spacialX];
	for (size_t i = 0; i < spacialX; ++i)
	{
		spacialStructure[i] = new cell*[spacialY];
		for (size_t j = 0; j < spacialY; ++j)
		{
			spacialStructure[i][j] = new cell[spacialZ];
		}
	}
}


SpacialStructure::~SpacialStructure()
{
	for (size_t i = 0; i < spacialX; ++i)
	{
		for (size_t j = 0; j < spacialY; ++j)
		{
			delete[] spacialStructure[i][j];
		}
		delete[] spacialStructure[i];
	}
	delete[] spacialStructure;
}


void SpacialStructure::clearTreeNodes()
{
	for (size_t i = 0; i < spacialX; ++i)
		for (size_t j = 0; j < spacialY; ++j)
			for (size_t k = 0; k < spacialZ; ++k)
				spacialStructure[i][j][k].treeNode.clear();
}

void SpacialStructure::clearAttractionNodes()
{
	for (size_t i = 0; i < spacialX; ++i)
		for (size_t j = 0; j < spacialY; ++j)
			for (size_t k = 0; k < spacialZ; ++k)
				spacialStructure[i][j][k].attractionNode.clear();
}

void SpacialStructure::clear() 
{
	clearAttractionNodes();
	clearTreeNodes();
}


glm::vec3 SpacialStructure::getCell(glm::vec3 location)
{
return glm::vec3(glm::clamp((int)floor((location.x - mins.x / (maxs.x - mins.x)) * spacialX), 0, spacialX - 1),
				 glm::clamp((int)floor((location.y - mins.y / (maxs.y - mins.y)) * spacialY), 0, spacialY - 1),
				 glm::clamp((int)floor((location.z - mins.z / (maxs.z - mins.z)) * spacialZ), 0, spacialZ - 1));
}


void SpacialStructure::addTreeNode(glm::vec3* location, int index)
{
	glm::vec3 cellPosition = getCell(*location);
	spacialStructure[(int)cellPosition.x][(int)cellPosition.y][(int)cellPosition.z].treeNode.emplace_back(index);
}

void SpacialStructure::addAttractionNode(glm::vec3* location)
{
	glm::vec3 cellPosition = getCell(*location);
	spacialStructure[(int)cellPosition.x][(int)cellPosition.y][(int)cellPosition.z].attractionNode.emplace_back(location);
}

void SpacialStructure::deleteAttractionNode(glm::vec3* location)
{
	glm::vec3 cellPosition = getCell(*location);
	std::vector<glm::vec3*>& nodelist = spacialStructure[(int)cellPosition.x][(int)cellPosition.y][(int)cellPosition.z].attractionNode;

	for (auto it = nodelist.begin(); it != nodelist.end(); it++)
	{
		if (*it == location) nodelist.erase(it);
	}
}

void SpacialStructure::deleteTreeNode(glm::vec3* location, int index)
{
	glm::vec3 cellPosition = getCell(*location);
	std::vector<int>& nodelist = spacialStructure[(int)cellPosition.x][(int)cellPosition.y][(int)cellPosition.z].treeNode;

	for (auto it = nodelist.begin(); it != nodelist.end(); it++)
	{
		if (*it == index) nodelist.erase(it);
	}
}


std::vector<glm::vec3*> SpacialStructure::getAttractionNodes(glm::vec3 location)
{
	glm::vec3 cellPosition = getCell(location);
	return spacialStructure[(int)cellPosition.x][(int)cellPosition.y][(int)cellPosition.z].attractionNode;
}

std::vector<int> SpacialStructure::getTreeNodes(glm::vec3 location)
{
	glm::vec3 cellPosition = getCell(location);
	return spacialStructure[(int)cellPosition.x][(int)cellPosition.y][(int)cellPosition.z].treeNode;
}