#include "LineInput.h"

#define _USE_MATH_DEFINES
#include <math.h>

LineInput::LineInput()
	: drawing(false)
{
}


LineInput::~LineInput()
{
}

void LineInput::update(float x, float y, bool isDown)
{
	if (!drawing && isDown)
	{
		if (lines.size() == 2)
		{
			lines.clear();
			surface.clear();
		}

		lines.emplace_back();
		lines.back().points.emplace_back(x, y, 0);
		drawing = true;
	}
	else if (drawing && !isDown)
	{
		drawing = false;
		if (lines.size() == 2)
			createSurface();
	}


	if (drawing)
	{
		glm::vec3 mousePos(x, y, 0);
		if (glm::distance(lines.back().points.back(), mousePos) > 0.01f)
		{
			lines.back().points.emplace_back(mousePos);
		}
	}

}

std::vector<Line>& LineInput::getLines()
{
	return lines;
}

std::vector<glm::vec3>& LineInput::getSurface()
{
	return surface;
}


void LineInput::createSurface()
{
	std::vector<glm::vec3> midPoint;

	const float inc = 0.01f;

	for (float u = 0.f; u <= 1.f; u += inc)
	{
		auto v1 = lines[0].parameterize(u);
		auto v2 = lines[1].parameterize(u);
		midPoint.emplace_back((v1 + v2)*0.5f);
	}

	std::vector<glm::vec3> triPoints;
	for (float u = 0.f; u <= 1.f; u += inc)
	{
		auto u1 = lines[0].parameterize(u);
		auto u2 = lines[1].parameterize(u);
		auto mid = (u1 + u2)*0.5f;

		float xd = std::abs(glm::dot(glm::vec3(1, 0, 0), glm::normalize(u1 - u2)));
		float yd = sqrt(1.f - xd * xd);

		for (float v = 0.f; v <= 2 * M_PI; v += inc * M_PI)
		{
			float t;
			if (v < M_PI)
				t = v / M_PI;
			else
				t = 2.f - (v / M_PI);

			auto v1 = u1 - mid;
			auto v2 = mid - u2;

			auto p = v1 * (1.f - t) + v2 * t;

			auto px = cos(v) * p.x;
			auto py = cos(v) * p.y;
			auto pz = sin(v) * p.x + sin(v) * p.y;

			triPoints.emplace_back(mid.x + px, mid.y + py, mid.z + pz);
		}
	}


	surface = triPoints;
}