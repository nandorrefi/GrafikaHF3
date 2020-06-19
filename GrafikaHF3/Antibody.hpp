#pragma once
#include "Object.hpp"

class Antibody : public Object{
	Tetrahedron* bodyShape;
	std::vector<Tetrahedron*> childrenShapes;
	std::vector<Object*> children;

	float scaleHeight = 1.0f;
	vec3 velocity;

	enum Likeliness {
		POSITIVE, NEGATIVE, NO_PREFERENCE
	};

	Likeliness xDir = NO_PREFERENCE, yDir = NO_PREFERENCE, zDir = NO_PREFERENCE;

	float getRandNum(Likeliness l) {

		float val = (float)(rand() % 1000) / 10000;

		if (l == POSITIVE) {
			if (rand() % 100 < 25)
				val *= -1;

			return val;
		}
		else if (l == NEGATIVE) {
			if (rand() % 100 >= 25)
				val *= -1;

			return val;
		}
		else {
			if (rand() % 100 >= 50)
				val *= -1;

			return val;
		}
	}

	vec3 randomVec3(Likeliness x, Likeliness y, Likeliness z) {
		return vec3(getRandNum(x), getRandNum(y), getRandNum(z));
	}

	void genChildren(Tetrahedron* parent, int levels = 0) {
		if (levels == 2)
			return;

		int startIndex = (levels == 0) ? 0 : 1;

		for (int i = startIndex; i < 4; ++i) {
			Triangle side = parent->getSide(i);

			vec3 positions[4];

			for (int j = 2; j >= 0; --j) {
				int jNext;

				if (j - 1 == -1)
					jNext = 2;
				else
					jNext = j - 1;

				vec3 newPosition = getMidpoint(side.getPosition(j), side.getPosition(jNext));
				positions[j] = newPosition;
			}

			vec3 midpoint = getMidpoint(positions[0], positions[1]);
			vec3 median = positions[2] - midpoint;

			float medianLength = length(median);
			float sideLength = length(positions[0] - positions[1]);

			float height = sideLength * sideLength - (2 * medianLength / 3) * (2 * medianLength / 3);

			height = sqrtf(height) * scaleHeight;

			int sign;
			if (levels % 2 == 0)
				sign = 1;
			else
				sign = -1;

			positions[3] = side.getCentroid() + sign * normalize(side.getNormal()) * height;

			Tetrahedron* childShape = new Tetrahedron(positions);
			Object* child = new Object(shader, material, texture, childShape);

			children.push_back(child);
			childrenShapes.push_back(childShape);

			genChildren(childShape, levels + 1);
		}
	}

	void deleteChildren() {

		for (int i = 0; i < children.size(); ++i) {
			delete childrenShapes[i];
			delete children[i];
		}

		childrenShapes.clear();
		children.clear();
	}

public:
	Antibody(Shader* shader, Material* material, Texture* texture) : Object(shader, material, texture, NULL) {
		
		bodyShape = new Tetrahedron();
		geometry = bodyShape;

		rotationAxis = vec3(1, 1, 1);
		translation = vec3(1.75, 0, 0);
		scale = vec3(0.75, 0.75, 0.75);

		genChildren(bodyShape);

		
	}
	~Antibody() {
		delete bodyShape;
		deleteChildren();
	}

	vec3 getOrigin() {
		return translation;
	}

	float getHitRadius() {
		return length(bodyShape->getSide(0).getPosition(0) - getOrigin());
	}

	void Draw(RenderState& state, const mat4& parentM = identityMatrix, const mat4& parentMinv = identityMatrix) {
		Object::Draw(state);

		mat4 M;
		mat4 Minv;
		SetModelingTransform(M, Minv);

		for (int i = 0; i < children.size(); ++i) {
			children[i]->Draw(state, M, Minv);
		}
	}

	void Animate(float tstart, float tend) {
		velocity = randomVec3(xDir, yDir, zDir);
		translation = translation + velocity;

		scaleHeight = sinf(tend*4)/2 + 1.5f;
		deleteChildren();
		genChildren(bodyShape);
		
		rotationAngle = 0.8f * tend;
	}

	void onKeyboard(char key) {
		if (key == 'x')
			xDir = POSITIVE;
		else if (key == 'X')
			xDir = NEGATIVE;
		else if (key == 'y')
			yDir = POSITIVE;
		else if (key == 'Y')
			yDir = NEGATIVE;
		if (key == 'z')
			zDir = POSITIVE;
		else if (key == 'Z')
			zDir = NEGATIVE;
	}

	void onKeyboardUp(char key) {
		if (key == 'x' || key == 'X')
			xDir = NO_PREFERENCE;
		else if (key == 'y' || key == 'Y')
			yDir = NO_PREFERENCE;
		if (key == 'z' || key == 'Z')
			zDir = NO_PREFERENCE;
	}
};