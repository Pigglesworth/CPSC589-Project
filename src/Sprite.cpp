#include "Sprite.h"

#include <iostream>

Sprite::Sprite(int width, int height, int bytes_per_pixel, const unsigned char* pixelData)
{
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	GLint format;
	if (bytes_per_pixel == 3)
		format = GL_RGB8;
	else
		format = GL_RGBA;


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
}


Sprite::~Sprite()
{
}
