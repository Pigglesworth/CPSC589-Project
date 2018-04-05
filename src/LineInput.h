#pragma once

#include <glm\glm.hpp>
#include <vector>
#include <algorithm>

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
	LineInput();
	~LineInput();

	void update(float mouseX, float mouseY, bool mouseIsDown);

	std::vector<Line>& getLines();
	std::vector<glm::vec3>& getSurface();
private:
	void createSurface();

	bool drawing;
	std::vector<Line> lines;
	std::vector<glm::vec3> surface;
};

