#include <gl\glew.h>
#include <GLFW\glfw3.h>

#include "Window.h"
#include "LineInput.h"

int main()
{
	Window window;
	LineInput lineDrawer;

	std::vector<glm::vec3> tri;
	tri.emplace_back(0, 0, 0);
	tri.emplace_back(1, 0, 0);
	tri.emplace_back(1, 1, 0);


	while (window.isOpen())
	{
		{
			auto mouse = window.getMousePosition();
			lineDrawer.update(mouse.first, mouse.second, window.getMouseDown());

			auto& lines = lineDrawer.getLines();
			for (auto& line : lines)
			{
				window.renderObject(line.points, GL_LINE_STRIP);
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

				window.renderObject(test, GL_LINE_STRIP);
			}

			window.renderObject(lineDrawer.getSurface(), lineDrawer.getSurfaceNormals(), lineDrawer.getSurfaceIndices());
			window.renderObject(lineDrawer.getVolumePoints(), GL_POINTS);


		}



		
		window.render();
	}

}