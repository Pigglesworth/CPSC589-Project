#include "Window.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

bool Window::doneInit = false;

Window::Window()
	: mouseX(0.f), mouseY(0.f), mouseIsDown(false)
	, cameraPosition(0,0,1)
{
	if (!doneInit)
		doInit();

	glfwWindowHint(GLFW_DEPTH_BITS, 8);

	window = glfwCreateWindow(1024, 768, "CPSC 589 Tree Generator", NULL, NULL);

	if (!doneInit)
	{
		glfwMakeContextCurrent(window);

		glewExperimental = GL_TRUE;
		glewInit();
	}
	doneInit = true;

	glGenVertexArrays(1, &vao);

	glfwSetWindowUserPointer(window, this);

	auto cursorLambda = [](GLFWwindow* w, double x, double y) 
	{ static_cast<Window*>(glfwGetWindowUserPointer(w))->handleMoveMouse(w, x, y); };
	glfwSetCursorPosCallback(window, cursorLambda);

	auto mouseLambda = [](GLFWwindow* w, int b, int a, int m)
	{ static_cast<Window*>(glfwGetWindowUserPointer(w))->handleMouseState(w, b, a, m); };
	glfwSetMouseButtonCallback(window, mouseLambda);

	auto keyLambda = [](GLFWwindow* w, int k, int s, int a, int m)
	{ static_cast<Window*>(glfwGetWindowUserPointer(w))->handleKeyState(w, k, s, a, m); };
	glfwSetKeyCallback(window, keyLambda);





	standardShader = new Shader("shaders/v.vert", "shaders/f.frag");
	meshShader = new Shader("shaders/v2.vert", "shaders/f2.frag");
}


Window::~Window()
{
}





void Window::renderObject(std::vector<glm::vec3>& posList, GLenum type)
{
	if (!posList.size())
		return;

	glDisable(GL_DEPTH_TEST);
	glPointSize(4.f);

	if (freeBuffers >= renderBuffers.size())
	{
		renderBuffers.emplace_back();
		glGenBuffers(1, &renderBuffers.back());
	}
	size_t thisBuffer = freeBuffers++;


	auto view = glm::lookAt(cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	auto pers = glm::perspective(glm::radians(60.f), 1024.f/768.f, 0.1f, 100.f);

	standardShader->useShader
	("view", pers * view);

	glBindBuffer(GL_ARRAY_BUFFER, renderBuffers[thisBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*posList.size(), &posList.front(), GL_DYNAMIC_DRAW);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glDrawArrays(type, 0, posList.size());
	glDisableVertexAttribArray(0);
}

void Window::renderObject(std::vector<glm::vec3>& posList, std::vector<glm::vec3>& normalList, std::vector<GLuint>& indexList)
{
	if (!posList.size() || !normalList.size())
		return;

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	while (freeBuffers+1 >= renderBuffers.size())
	{
		renderBuffers.emplace_back();
		glGenBuffers(1, &renderBuffers.back());
	}
	size_t posBuffer = freeBuffers++;
	size_t normBuffer = freeBuffers++;

	auto view = glm::lookAt(cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	auto pers = glm::perspective(glm::radians(60.f), 1024.f / 768.f, 0.1f, 100.f);

	meshShader->useShader
	("view", pers * view);

	glBindBuffer(GL_ARRAY_BUFFER, renderBuffers[posBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*posList.size(), &posList.front(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, renderBuffers[normBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*normalList.size(), &normalList.front(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);

	glDrawElements(GL_TRIANGLES, indexList.size(), GL_UNSIGNED_INT, &indexList.front());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
}






void Window::render()
{
	glfwSwapBuffers(window);
	glfwPollEvents();

	freeBuffers = 0;
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::isOpen()
{
	return !glfwWindowShouldClose(window);
}

std::pair<float, float> Window::getMousePosition()
{
	return std::make_pair(mouseX, mouseY);
}

bool Window::getMouseDown()
{
	return mouseIsDown;
}








void Window::handleKeyState(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A)
	{
		auto newPos = glm::vec4(cameraPosition,0.f) * glm::rotate(glm::mat4(1.f), .1f, glm::vec3(0, 1, 0));
		cameraPosition = glm::vec3(newPos);
	}
}

void Window::handleMoveMouse(GLFWwindow * window, double x, double y)
{
	auto view = glm::lookAt(glm::vec3(0.f,0.f,1.f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	auto pers = glm::perspective(glm::radians(60.f), 1024.f / 768.f, 0.1f, 100.f);

	auto inv = glm::inverse(pers * view);

	glm::vec4 v(0.f, 0.f, 0.f, 1.f);
	v.x = ((x / 1024) - 0.5f)*2.f;
	v.y = ((y / 768) - 0.5f)*-2.f;

	v = inv * v;

	mouseX = v.x;
	mouseY = v.y;
}

void Window::handleMouseState(GLFWwindow * window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		cameraPosition = glm::vec3(0, 0, 1);
		mouseIsDown = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseIsDown = false;
	}
}








void Window::doInit()
{
	glfwInit();
}