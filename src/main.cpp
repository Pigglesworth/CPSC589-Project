#include <gl\glew.h>
#include <GLFW\glfw3.h>

#include "Window.h"
#include "LineInput.h"
#include "SkeletonGenerator.h"
#include "ObjExporter.h"

bool exportObj = false;

int main()
{
	Window window;
	LineInput lineDrawer;
	SkeletonGenerator skeletal;
	ObjExporter objer;

	while (window.isOpen())
	{
		{
			auto mouse = window.getMousePosition();
			lineDrawer.update(mouse.first, mouse.second, window.getMouseDown());

			auto& lines = lineDrawer.getLines();
			for (auto& line : lines)
			{
				window.renderObject(line.points, glm::vec3(1.f,0.f,1.f), GL_LINE_STRIP);
			}


			if (lines.size() > 1)
			{
				std::vector<glm::vec3> test;
				for (float u = 0.f; u < 1.f; u += 0.01f)
				{
					auto v1 = lines[0].parameterize(u);
					auto v2 = lines[1].parameterize(u);
					test.emplace_back((v1 + v2)*0.5f);
				}

				window.renderObject(test, glm::vec3(1.f, 0.f, 1.f), GL_LINE_STRIP);


		
				if (lineDrawer.getVolumePoints().size())
				{

					if (!skeletal.hasStarted())
					{
						glm::vec3 bottom(FLT_MAX);
						for (auto& point : test)
						{
							if (point.y < bottom.y)
								bottom = point;
						}
						bottom.y -= 0.1;

						skeletal.begin(lineDrawer.getVolumePoints(), glm::vec3(bottom));
					}
					else
					{
						skeletal.step();
					}
				}
				else
				{
					skeletal.clear();
				}

				if (skeletal.isFinished())
				{
					window.renderObject(skeletal.getMeshPoints(), skeletal.getMeshNormals(), skeletal.getMeshIndices());
					if (!exportObj)
					{
						objer.exportMesh
							( skeletal.getMeshPoints(), skeletal.getMeshTexCoords()
							, skeletal.getMeshNormals(), skeletal.getMeshIndices());

						exportObj = true;
					}
				}
				else
				{
					window.renderObject(lineDrawer.getSurface(), lineDrawer.getSurfaceNormals(), lineDrawer.getSurfaceIndices());
					window.renderObject(lineDrawer.getVolumePoints(), glm::vec3(1.f, 0.f, 1.f), GL_POINTS);

					window.renderObject(skeletal.getNodePositions(), skeletal.getNodeIndices(), glm::vec3(0.f, 0.f, 1.f), GL_LINES);
				}

			}

		}



		
		window.render();
	}

}