#include "Slider.h"

#include <glm/gtc/matrix_transform.hpp>

Slider::Slider(float _min, float _max, glm::vec2 _pos)
	: min(_min), max(_max), val(0.5f)
	, pos(_pos)
	, mouseAlreadyPressed(false), beingHeld(false)
{
}


Slider::~Slider()
{
}





void Slider::update(bool mousePressed, float mouseX, float mouseY)
{
	bool mousedOver = false;

	if (mousePressed)
		mousedOver = false;

	if (mouseX >= pos.x && mouseX <= pos.x + 0.55f && mouseY >= pos.y && mouseY <= pos.y + 0.05f)
		mousedOver = true;

	if (mousedOver && mousePressed && !mouseAlreadyPressed)
		beingHeld = true;

	if (beingHeld && !mousePressed)
		beingHeld = false;

	mouseAlreadyPressed = mousePressed;



	if (beingHeld)
	{
		val = 2*(mouseX - (pos.x));
		val = val >= 0.f ? val : 0.f;
		val = val <= 1.f ? val : 1.f;
	}
}



void Slider::render(Window& window)
{
	std::vector<glm::vec3> rect;
	rect.emplace_back(0.f,0.f,0.f);
	rect.emplace_back(0.f, 1.f, 0.f);
	rect.emplace_back(1.f, 0.f, 0.f);
	rect.emplace_back(1.f, 1.f, 0.f);

	auto transform = glm::scale(glm::mat4(), glm::vec3(0.5f, 0.05f,1.f));
	for (auto& pt : rect)
	{
		pt = glm::vec3(glm::vec4(pt,1.f)*transform);
		pt += glm::vec3(pos, 0.f);
	}
	window.renderObject(rect, glm::vec3(0.6f, 0.6f, 0.6f), GL_TRIANGLE_STRIP, true);

	rect.clear();


	rect.emplace_back(0.f, 0.f, 0.f);
	rect.emplace_back(0.f, 1.f, 0.f);
	rect.emplace_back(1.f, 0.f, 0.f);
	rect.emplace_back(1.f, 1.f, 0.f);

	transform = glm::scale(glm::mat4(), glm::vec3(0.05f, 0.05f, 1.f));
	for (auto& pt : rect)
	{
		pt = glm::vec3(glm::vec4(pt, 1.f)*transform);
		pt += glm::vec3(pos, 0.f);
		pt.x += val*0.45f;
	}
	window.renderObject(rect, glm::vec3(1.f, 1.f, 1.f), GL_TRIANGLE_STRIP, true);

}



float Slider::getValue()
{
	return min + (max-min)*val*val;
}

bool Slider::isHeld()
{
	return beingHeld;
}
