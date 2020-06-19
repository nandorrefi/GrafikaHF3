#pragma once
#include "Light.h"

class PlainTexture : public Texture {
public:
	PlainTexture() : Texture() {
		std::vector<vec4> image(1);
		image[0] = vec4(0.15, 0.15, 1, 1);
		create(1, 1, image, GL_NEAREST);
	}
};

class SquaredTexture : public Texture {
public:
	SquaredTexture(const int width = 0, const int height = 0) : Texture() {
		std::vector<vec4> image(width * height);
		const vec4 color1(0, 1, 0, 1), color2(0, 0, 0, 1);

		for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
			image[y * width + x] = (x & 1) || (y & 1) ? color1 : color2;
		}
		create(width, height, image, GL_NEAREST);
	}
};

class VerticalLinearTexture : public Texture {	
public:
	VerticalLinearTexture(const int width = 0, const int height = 0) : Texture() {
		std::vector<vec4> image(width * height);
		const vec4 color1(0.8, 0.8, 0.15, 1), color2(0.4, 0, 0, 1);
		for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
			image[y * width + x] = (x & 1) ? color1 : color2;
		}
		create(width, height, image, GL_NEAREST);
	}
};

class HorizontalLinearTexture : public Texture {
public:
	HorizontalLinearTexture(const int width = 0, const int height = 0) : Texture() {
		std::vector<vec4> image(width * height);
		const vec4 color1(1, 0, 0, 1), color2(0, 0, 0, 1);
		for (int x = 0; x < width; x++) for (int y = 0; y < height; y++) {
			image[y * width + x] = (y & 1) ? color1 : color2;
		}
		create(width, height, image, GL_NEAREST);
	}
};