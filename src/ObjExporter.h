#pragma once

#include <gl\glew.h>
#include <glm\glm.hpp>
#include <vector>

class ObjExporter
{
public:
	ObjExporter();
	~ObjExporter();

	void exportMesh
	(std::vector<glm::vec3>& pos, std::vector<glm::vec2>& tex
	, std::vector<glm::vec3>& norm, std::vector<GLuint>& indices);
};

