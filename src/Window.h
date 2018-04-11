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

	void renderObject
		( std::vector<glm::vec3>& posList, glm::vec3 colour = glm::vec3(1.f), GLenum type = GL_TRIANGLES);
	void renderObject
		( std::vector<glm::vec3>& posList, std::vector<GLuint>& indexList
		, glm::vec3 colour = glm::vec3(1.f), GLenum type = GL_TRIANGLES);
	void renderObject(std::vector<glm::vec3>& posList, std::vector<glm::vec3>& normalList, std::vector<GLuint>& indexList);

	void render();
	bool isOpen();

	std::pair<float, float> getMousePosition();
	bool getMouseDown();
private:
	static void doInit();
	static bool doneInit;

	void handleKeyState(GLFWwindow* window, int key, int scancode, int action, int mods);
	void handleMoveMouse(GLFWwindow* window, double x, double y);
	void handleMouseState(GLFWwindow* window, int button, int action, int mods);
	void handleWindowResize(GLFWwindow* window, int w, int h);

	glm::mat4 makeVPMatrix();

	int winWidth, winHeight;
	float mouseX, mouseY;
	bool mouseIsDown;

	glm::vec3 cameraPosition;

	GLuint vao;
	std::vector<GLuint> renderBuffers;
	size_t freeBuffers;

	GLFWwindow* window;
	Shader* standardShader;
	Shader* meshShader;
};

