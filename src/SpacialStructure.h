#pragma once

#include <glm\glm.hpp>
#include <vector>

#include <gl\glew.h>

class SpacialStructure
{
public:
	SpacialStructure();
	SpacialStructure(glm::vec3 dimensions);
	SpacialStructure(glm::vec3 dimensions, glm::vec3 mins, glm::vec3 maxs);
	~SpacialStructure();

	void addTreeNode(glm::vec3*, int index);
	void addAttractionNode(glm::vec3*);

	void deleteTreeNode(glm::vec3*, int index);
	void deleteAttractionNode(glm::vec3*);

	void clearTreeNodes();
	void clearAttractionNodes();
	void clear();

	std::vector<glm::vec3*> getAttractionNodes(glm::vec3);
	std::vector<int> getTreeNodes(glm::vec3);

	glm::vec3 getSpacing();

private:
	
	int spacingX, spacingY, spacingZ;

	glm::vec3 mins, maxs;

	struct cell
	{
		std::vector<int> treeNode;
		std::vector<glm::vec3*> attractionNode;
	};

	glm::vec3 getCell(glm::vec3);

	cell*** spacialStructure;
};