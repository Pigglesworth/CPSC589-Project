#pragma once

#include "SpacialStructure.h"
#include <gl\glew.h>
#include <glm\glm.hpp>

#include <vector>

class SkeletonGenerator
{
public:
	SkeletonGenerator(SpacialStructure* space);
	~SkeletonGenerator();

	
	void begin(std::vector<glm::vec3>& points, glm::vec3 startingPoint = glm::vec3(0.f));
	void step();
	bool hasStarted();
	bool isFinished();
	void forceFinished();
	void clear();

	void smoothAndUpdate();

	void setCullDistance(float d);
	void setSearchDistance(float d);
	void setNodeDistance(float d);
	void setMaxRadius(float d);
	void setMinRadius(float d);

	void generateMesh();


	std::vector<glm::vec3>& getNodePositions();
	std::vector<GLuint>& getNodeIndices();

	std::vector<glm::vec3>& getMeshPoints();
	std::vector<glm::vec2>& getMeshTexCoords();
	std::vector<glm::vec3>& getMeshNormals();
	std::vector<GLuint>& getMeshIndices();

private:
	void calculateDepths();
	float getDepth(size_t i);

	void calculateSizes();
	float getSize(size_t);
	
	std::vector<std::pair<glm::vec3, bool>> attractionPoints;
	size_t activePoints, stepCount;

	void addNode(glm::vec3 position, size_t parent);

	void smooth();

	struct Node
	{
		glm::vec3 nodePoint;
		glm::vec3 nodeWeight;
		size_t nodeParent;
		std::vector<size_t> nodeChildren;
		float depth;
		float size;
		bool trunk;
	};

	void createRevolution(size_t point1, size_t point2);

	std::vector<Node> nodes;
	std::vector<GLuint> nodeIndices;
	std::vector<glm::vec3> nodePositions;

	int trunks;
	bool nodeAdded;

	bool finish;

	std::vector<glm::vec3> meshPoints;
	std::vector<glm::vec2> meshTexCoords;
	std::vector<glm::vec3> meshNormals;
	std::vector<GLuint> meshIndices;

	SpacialStructure * spacialStructure;

	int getNodeCount();

	size_t maxDepth;
	size_t minDepth;

	float cullDistance;
	float searchDistance;
	float nodeDistance;

	float minRadius;
	float maxRadius;
};

