#include "ObjExporter.h"

#include <fstream>
#include <string>

ObjExporter::ObjExporter()
{
}


ObjExporter::~ObjExporter()
{
}

void ObjExporter::exportMesh
( std::vector<glm::vec3>& pos, std::vector<glm::vec2>& tex
, std::vector<glm::vec3>& norm, std::vector<GLuint>& indices)
{
	int fileEnding = 0;
	std::string filePath("tree" + std::to_string(fileEnding) + ".obj");
	std::ofstream file(filePath);
	while (file.is_open() && false)
	{
		fileEnding++;
		filePath = "tree" + std::to_string(fileEnding) + ".obj";
		file.open(filePath);
	}

	file.setf(std::ios_base::fixed);
	for (auto& p : pos)
	{
		file << "v " << p.x << " " << p.y << " " << p.z << " " << std::endl;
	}

	for (auto& t : tex)
	{
		file << "vt " << t.x << " " << t.y << " " << std::endl;
	}

	for (auto& n : norm)
	{
		file << "vn " << n.x << " " << n.y << " " << n.z << " " << std::endl;
	}

	for (size_t i = 0; i+2 < indices.size(); i += 3)
	{
		file << "f " << indices[i]+1 << "/" << indices[i]+1 << "/" << indices[i]+1
			<< " " << indices[i + 1]+1 << "/" << indices[i + 1]+1 << "/" << indices[i + 1]+1
			<< " " << indices[i + 2]+1 << "/" << indices[i + 2]+1 << "/" << indices[i + 2]+1 << " " << std::endl;
	}

	file.close();
}
