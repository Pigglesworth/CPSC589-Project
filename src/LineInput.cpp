#include "LineInput.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include <chrono>

LineInput::LineInput(SpacialStructure* space)
	: drawing(false), spacialStructure(space), volumePointCount(1000)
    , finished(false)
{
}


LineInput::~LineInput()
{
}

void LineInput::update(float x, float y, bool isDown)
{
	if (!drawing && isDown)
	{
		lines.emplace_back();
		lines.back().points.emplace_back(x, y, 0);
		drawing = true;
	}
	else if (drawing && !isDown)
	{
		drawing = false;
		if (lines.size() > 0 && lines.size() % 2 == 0)
		{
			createSurface();
			generateVolumePoints(volumePointCount);
		}
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

void LineInput::setDensity(size_t density)
{
	volumePointCount = density;
}

void LineInput::setFinished(bool _finished)
{
    finished = _finished;
}

void LineInput::clear()
{
    finished = false;
    lines.clear();
    surfaces.clear();
    volumePoints.clear();
}

bool LineInput::isFinished()
{
    return finished;
}

bool LineInput::hasBegun()
{
    return lines.size() > 0;
}

std::vector<Line>& LineInput::getLines()
{
	return lines;
}

std::vector<LineInput::RevolutionSurface>& LineInput::getSurfaces()
{
	return surfaces;
}


std::vector<glm::vec3>& LineInput::getVolumePoints()
{
	return volumePoints;
}


void LineInput::createSurface()
{
    surfaces.emplace_back();
    auto& newSurface = surfaces.back();

	const size_t steps = 100;
	const float inc = 1.f / (steps-1);

	for (size_t ui = 0; ui < steps; ui++)
	{
		for (size_t vi = 0; vi < steps; vi++)
		{
			float u = ((float)ui) * inc;
			float v = ((float)vi) * inc * 2 * M_PI;

            newSurface.surface.emplace_back(getVolumePoint(u, v, 1.0f));
            newSurface.surfaceNormal.emplace_back(getVolumeNormal(u, v, 1.0f));
			
			if (ui + 1 < steps && vi + 1 < steps)
			{
                newSurface.surfaceIndices.emplace_back(ui*steps + vi);
                newSurface.surfaceIndices.emplace_back(ui*steps + vi + 1);
                newSurface.surfaceIndices.emplace_back((ui + 1)*steps + vi);

                newSurface.surfaceIndices.emplace_back(ui*steps + vi + 1);
                newSurface.surfaceIndices.emplace_back((ui + 1)*steps + vi + 1);
                newSurface.surfaceIndices.emplace_back((ui + 1)*steps + vi);
			}
		}
	}
}

void LineInput::generateVolumePoints(size_t count)
{
//	volumePoints.clear();
	volumePoints.reserve(count + volumePoints.size());
	if(spacialStructure) spacialStructure->clearAttractionNodes();

	std::default_random_engine generator;
	generator.seed(std::chrono::high_resolution_clock().now().time_since_epoch().count());
	std::uniform_real_distribution<float> distribution(0.0, 1.0);

	for (size_t i = 0; i < count; i++)
	{
		float u = distribution(generator);
		float v = distribution(generator) * 2 * M_PI;
		float w = std::sqrt(distribution(generator));

		volumePoints.emplace_back(getVolumePoint(u, v, w));
	}
}


glm::vec3 LineInput::getVolumePoint(float u, float v, float w)
{
    size_t lastLine = lines.size() - 1;

	auto u1 = lines[lastLine-1].parameterize(u);
	auto u2 = lines[lastLine].parameterize(u);
	auto mid = (u1 + u2)*0.5f;

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

	return glm::vec3(mid.x + px*w, mid.y + py*w, mid.z + pz*w);
}

glm::vec3 LineInput::getVolumeNormal(float u, float v, float w)
{
	if (w == 0.f)
		return glm::vec3(0.f, 0.f, 0.f);

	auto u1 = lines[0].parameterize(u);
	auto u2 = lines[1].parameterize(u);
	auto mid = (u1 + u2)*0.5f;

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

	glm::vec3 point(px, mid.y + py, mid.z + pz);

	return glm::normalize(point-mid);
}