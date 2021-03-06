#pragma once

#include "SpacialStructure.h"
#include <glm\glm.hpp>
#include <vector>
#include <algorithm>

#include <gl\glew.h>

struct Line
{
	std::vector<glm::vec3> points;

	glm::vec3 parameterize(float u)
	{
		if (points.size() < 2)
			return glm::vec3(0, 0, 0);

		u = std::max(0.f, std::min(1.f, u));

		float interval = 1.f / points.size();
		size_t index = static_cast<size_t>(u * points.size());

		index = std::min(index, points.size() - 2);

		float delta = std::fmod(u, interval) * points.size();


		return points[index] * (1.f-delta) + points[index + 1] * delta;
	}
};



class LineInput
{
public:
	LineInput(SpacialStructure* space);
	~LineInput();

	void update(float mouseX, float mouseY, bool mouseIsDown);
	void setDensity(size_t density);

    void setFinished(bool finished);
    void clear();

    bool isFinished();
    bool hasBegun();

	std::vector<Line>& getLines();

    struct RevolutionSurface
    {
        std::vector<glm::vec3> surface;
        std::vector<glm::vec3> surfaceNormal;
        std::vector<GLuint> surfaceIndices;
    };

    std::vector<RevolutionSurface>& getSurfaces();

	std::vector<std::vector<glm::vec3>>& getVolumePoints();

private:
	void createSurface();
	void generateVolumePoints(size_t count);

	glm::vec3 getVolumePoint(float u, float v, float w);
	glm::vec3 getVolumeNormal(float u, float v, float w);

    bool finished;
	bool drawing;
	std::vector<Line> lines;
    std::vector<RevolutionSurface> surfaces;

	std::vector<std::vector<glm::vec3>> volumePoints;

	size_t volumePointCount;
    bool densityChanged;

	SpacialStructure* spacialStructure;
};

