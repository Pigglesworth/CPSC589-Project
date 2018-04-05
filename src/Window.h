#pragma once

#include <gl\glew.h>
#include <glm\glm.hpp>
#include <GLFW\glfw3.h>
#include <vector>

#include "Shader.h"

class Window
{
public:
	Window();
	~Window();



	void renderObject(std::vector<glm::vec3>& posList, GLenum type = GL_TRIANGLES);

	void render();
	bool isOpen();

	std::pair<float, float> getMousePosition();
	bool getMouseDown();
private:
	static void doInit();
	static bool doneInit;

	void handleMoveMouse(GLFWwindow* window, double x, double y);
	void handleMouseState(GLFWwindow* window, int button, int action, int mods);
	float mouseX, mouseY;
	bool mouseIsDown;



	GLuint vao;
	std::vector<GLuint> renderBuffers;
	size_t freeBuffers;

	GLFWwindow* window;
	Shader* standardShader;
};

