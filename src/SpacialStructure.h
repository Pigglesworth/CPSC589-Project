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

private:
	int spacialX, spacialY, spacialZ;
	glm::vec3 mins, maxs;

	struct cell
	{
		std::vector<int> treeNode;
		std::vector<glm::vec3*> attractionNode;
	};

	glm::vec3 getCell(glm::vec3);

	cell*** spacialStructure;
};


/*
class S
{
public:
	static S& getInstance()
	{
		static S    instance; // Guaranteed to be destroyed.
							  // Instantiated on first use.
		return instance;
	}
private:
	S() {}                    // Constructor? (the {} brackets) are needed here.

							  // C++ 03
							  // ========
							  // Don't forget to declare these two. You want to make sure they
							  // are unacceptable otherwise you may accidentally get copies of
							  // your singleton appearing.
	S(S const&);              // Don't Implement
	void operator=(S const&); // Don't implement

							  // C++ 11
							  // =======
							  // We can use the better technique of deleting the methods
							  // we don't want.
public:
	S(S const&) = delete;
	void operator=(S const&) = delete;

	// Note: Scott Meyers mentions in his Effective Modern
	//       C++ book, that deleted functions should generally
	//       be public as it results in better error messages
	//       due to the compilers behavior to check accessibility
	//       before deleted status
};
*/