#pragma once
#include "Hitable.hpp"

mat4 identityMatrix = mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, 1, 0), vec4(0, 0, 0, 1));

struct Object : public Hitable {
protected:
	Shader* shader;
	Material* material;
	Texture* texture;
	Geometry* geometry;
	vec3 scale, translation, rotationAxis;
	float rotationAngle;

public:

	Object(Shader* _shader, Material* _material, Texture* _texture, Geometry* _geometry) :
		scale(vec3(1, 1, 1)), translation(vec3(0, 0, 0)), rotationAxis(0, 0, 1), rotationAngle(0) {
		shader = _shader;
		texture = _texture;
		material = _material;
		geometry = _geometry;
	}

	virtual ~Object() {

	}

	float getHitRadius() { return 0; }
	vec3 getOrigin() { return translation; }

	void transform(vec3 scale, vec3 translation, float rotationAngle, vec3 rotationAxis) {
		this->scale = scale;
		this->translation = translation;
		this->rotationAngle = rotationAngle;
		this->rotationAxis = rotationAxis;
	}

	void SetModelingTransform(mat4& M, mat4& Minv) {
		M = ScaleMatrix(scale) * RotationMatrix(rotationAngle, rotationAxis) * TranslateMatrix(translation);
		Minv = TranslateMatrix(-translation) * RotationMatrix(-rotationAngle, rotationAxis) * ScaleMatrix(vec3(1 / scale.x, 1 / scale.y, 1 / scale.z));
	}

	virtual void Draw(RenderState& state, const mat4& parentM = identityMatrix, const mat4& parentMinv = identityMatrix) {
		mat4 M, Minv;
		SetModelingTransform(M, Minv);
		M = M * parentM;
		Minv = parentMinv * Minv;
		state.M = M;
		state.Minv = Minv;
		state.MVP = state.M * state.V * state.P;
		state.material = material;
		state.texture = texture;
		shader->Bind(state);
		geometry->Draw();
	}

	void setT(float time) {
		geometry->setT(time);
	}

	virtual void Animate(float tstart, float tend) { }
};