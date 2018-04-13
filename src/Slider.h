#pragma once

#include "Window.h"
#include "glm\glm.hpp"

class Slider
{
public:
	Slider(float min, float max, glm::vec2 pos);
	~Slider();

	void update(bool mousePressed, float mouseX, float mouseY);
	void render(Window& window);

	float getValue();
	bool isHeld();
private:
	bool mouseAlreadyPressed;
	bool beingHeld;
	float min, max, val;
	glm::vec2 pos;
};

