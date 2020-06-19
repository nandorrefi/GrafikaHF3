#pragma once
#include "PhongShader.hpp"

class Hitable {
	virtual float getHitRadius() = 0;
	virtual vec3 getOrigin() = 0;
};