#include <gl\glew.h>
#include <GLFW\glfw3.h>

#include "Window.h"
#include "LineInput.h"
#include "SkeletonGenerator.h"
#include "ObjExporter.h"
#include "Slider.h"

bool exportObj = true;
bool smooth = false;

int main()
{
	Window window;
	SpacialStructure* space = new SpacialStructure(glm::vec3(10));
	LineInput lineDrawer(space);
	SkeletonGenerator skeletal(space);
	ObjExporter objer;

	Slider densitySlider(10.f,30000.f,glm::vec2(0.45f,0.85f));
	Slider cullDistanceSlider(0.01f, 0.1f, glm::vec2(0.45f, 0.75f));
	Slider searchDistanceSlider(0.005f, 0.2f, glm::vec2(0.45f, 0.65f));
	Slider nodeDistanceSlider(0.005f, 0.1f, glm::vec2(0.45f, 0.55f));

	int waitForPointPlacement = 0;


	while (window.isOpen())
	{
		{
			auto mouse = window.getMousePosition();
			auto mouseScreen = window.getMouseScreenPosition();
			auto mouseDown = window.getMouseDown();

			densitySlider.update(mouseDown, mouseScreen.first, mouseScreen.second);
			cullDistanceSlider.update(mouseDown, mouseScreen.first, mouseScreen.second);
			searchDistanceSlider.update(mouseDown, mouseScreen.first, mouseScreen.second);
			nodeDistanceSlider.update(mouseDown, mouseScreen.first, mouseScreen.second);

			if (densitySlider.isHeld() || cullDistanceSlider.isHeld() || nodeDistanceSlider.isHeld() || searchDistanceSlider.isHeld())
				mouseDown = false;

			lineDrawer.setDensity(densitySlider.getValue());
			skeletal.setCullDistance(cullDistanceSlider.getValue());
			skeletal.setSearchDistance(searchDistanceSlider.getValue());
			skeletal.setNodeDistance(nodeDistanceSlider.getValue());




			if (waitForPointPlacement == 2 && !mouseDown)
				waitForPointPlacement = 0;

			if(!waitForPointPlacement)
				lineDrawer.update(mouse.first, mouse.second, mouseDown);

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
						waitForPointPlacement = 1;

						if (mouseDown)
						{
							skeletal.begin(lineDrawer.getVolumePoints(), glm::vec3(mouse.first,mouse.second,0.f));
							waitForPointPlacement = 2;
						}
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
					if (smooth)
					{
						skeletal.smoothAndUpdate();
						smooth = false;
					}

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


		densitySlider.render(window);
		cullDistanceSlider.render(window);
		searchDistanceSlider.render(window);
		nodeDistanceSlider.render(window);

		window.render();
	}

}