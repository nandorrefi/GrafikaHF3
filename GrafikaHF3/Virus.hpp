#pragma once
#include "Antibody.hpp"

vec4 operator*(const mat4& m, const vec4& v) {
	vec4 result;

	result.x = dot(m.rows[0], v);
	result.y = dot(m.rows[1], v);
	result.z = dot(m.rows[2], v);
	result.w = dot(m.rows[3], v);

	return result;
}

struct Projection {
	Object* object;
	vec2 texcoord;
};

class Virus : public Object {
	ParamSurface* bodyShape;
	Geometry* projectionShape;
	std::vector<Projection> corona;
	Material* coronaMaterial;
	Texture* coronaTexture;

	bool killed = false;

	void transformProjectionObject(Object* obj, VertexData vd) {
		vec3 currDir = vec3(0, 0, 1);
		vec3 normal = normalize(vd.normal);

		vec3 projScale = scale / 2.5;
		vec3 projTranslation = vd.position - normal * length(projScale);

		float angle = acosf(dot(currDir, normal));
		vec3 projRotationAxis = cross(currDir, normal);

		obj->transform(projScale, projTranslation, angle, projRotationAxis);
	}

	void genCorona() {
		int n = 20;
		int m = 10;

		float height = (float)M_PI;
		float width = (float)(2 * M_PI);

		float heightStep = height / m;
		float widthStep = width;

		for (float i = 0; i < height; i += heightStep) {
			VertexData sample = bodyShape->GenVertexData(0, i / height);
			float sinPhi = length(vec3(sample.position.x, sample.position.y, 0)) / length(sample.position);
			int N = n * sinPhi;

			if (N != 0)
				widthStep = width / N;

			for (float j = 0; j < width; j += widthStep) {
				float u = j / width;
				float v = i / height;

				VertexData bodyVd = bodyShape->GenVertexData(u, v);

				Object* projectionObject = new Object(shader, coronaMaterial, coronaTexture, projectionShape);

				transformProjectionObject(projectionObject, bodyVd);

				Projection projection;
				projection.object = projectionObject;
				projection.texcoord = vec2(u, v);

				corona.push_back(projection);
			}
		}
	}

	void updateCorona() {
		for (int i = 0; i < corona.size(); ++i) {
			VertexData bodyVd = bodyShape->GenVertexData(corona[i].texcoord.x, corona[i].texcoord.y);

			transformProjectionObject(corona[i].object, bodyVd);
		}
	}

public:
	Virus(Shader* shader, Material* bodyMaterial, Texture* bodyTexture, Material* coronaMaterial, Texture* coronaTexture)
	: Object(shader, bodyMaterial, bodyTexture, NULL) {
		projectionShape = new Tractricoid();
		bodyShape = new MorphingSphere();

		geometry = bodyShape;

		translation = vec3(-0.75, 0, 0);
		rotationAngle = M_PI / 2;
		rotationAxis = vec3(0, 1, 1);
		scale = vec3(0.35f, 0.35f, 0.35f);

		this->coronaMaterial = coronaMaterial;
		this->coronaTexture = coronaTexture;

		genCorona();
	}

	~Virus() {
		delete projectionShape;
		delete bodyShape;
		for (int i = 0; i < corona.size(); ++i)
			delete corona[i].object;
	}

	void kill() {
		killed = true;
	}

	vec3 getOrigin() {
		return translation;
	}

	float getHitRadius() {
		return scale.y;
	}

	void Draw(RenderState& state, const mat4& parentM = identityMatrix, const mat4& parentMinv = identityMatrix) {
		Object::Draw(state);
		mat4 M;
		mat4 Minv;
		SetModelingTransform(M, Minv);

		for (int i = 0; i < corona.size(); ++i) {
			corona[i].object->Draw(state, M, Minv);
		}
	}

	void Animate(float tstart, float tend) {
		if (!killed) {
			setT(tend * 2);
			updateCorona();

			rotationAngle = 0.8f * tend;			
		}
	}
	
};