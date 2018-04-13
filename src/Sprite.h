#pragma once

#include "GL/glew.h"

class Sprite
{
public:
	Sprite(int width, int height, int bytes_per_pixel, const unsigned char* pixelData);
	~Sprite();

	GLuint textureID;
};

