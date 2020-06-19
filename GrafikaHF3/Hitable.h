#pragma once
#include "PhongShader.h"

class Hitable {
	virtual float getHitRadius() = 0;
	virtual vec3 getOrigin() = 0;
};