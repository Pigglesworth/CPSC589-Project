#pragma once

#include "SpacialStructure.h"
#include <gl\glew.h>
#include <glm\glm.hpp>

#include <vector>

class SkeletonGenerator
{
public:
	SkeletonGenerator();
	~SkeletonGenerator();

	
	void begin(std::vector<glm::vec3>& points, glm::vec3 startingPoint = glm::vec3(0.f));
	void step();
	bool hasStarted();
	bool isFinished();
	void clear();


	std::vector<glm::vec3>& getNodePositions();
	std::vector<GLuint>& getNodeIndices();

	std::vector<glm::vec3>& getMeshPoints();
	std::vector<glm::vec2>& getMeshTexCoords();
	std::vector<glm::vec3>& getMeshNormals();
	std::vector<GLuint>& getMeshIndices();
private:
	std::vector<std::pair<glm::vec3, bool>> points;
	size_t activePoints, stepCount;

	void addNode(glm::vec3 position, size_t parent);

	void generateMesh();

	struct NodeList
	{
		std::vector<glm::vec3> nodePoints;
		std::vector<glm::vec3> nodeWeights;
		std::vector<size_t> nodeParents;
		std::vector<std::vector<size_t>> nodeChildren;
	};

	std::vector<std::pair<size_t,size_t>> makeRevolutionList(NodeList& nodes, size_t start = 0);
	void createRevolution(NodeList& nodes, size_t point1, size_t point2);

	NodeList nodes;
	std::vector<GLuint> nodeIndices;


	std::vector<glm::vec3> meshPoints;
	std::vector<glm::vec2> meshTexCoords;
	std::vector<glm::vec3> meshNormals;
	std::vector<GLuint> meshIndices;

	SpacialStructure * spacialStructure;
};

