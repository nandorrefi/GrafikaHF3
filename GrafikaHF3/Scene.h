#pragma once
#include "Virus.h"

class Scene {
	std::vector<Object*> objects;
	Camera camera;
	std::vector<Light> lights;
	Virus* virus;
	Antibody* antibody;

public:
	void Build() {
		camera.wEye = vec3(0, 0, 6);
		camera.wLookat = vec3(0, 0, 0);
		camera.wVup = vec3(0, 1, 0);

		Shader* phongShader = new PhongShader();

		Material* material0 = new Material;
		material0->kd = vec3(0.6f, 0.4f, 0.2f);
		material0->ks = vec3(4, 4, 4);
		material0->ka = vec3(0.1f, 0.1f, 0.1f);
		material0->shininess = 100;

		Material* material1 = new Material;
		material1->kd = vec3(0.8f, 0.6f, 0.4f);
		material1->ks = vec3(0.9f, 0.9f, 0.9f);
		material1->ka = vec3(0.2f, 0.2f, 0.2f);
		material1->shininess = 30;

		Material* boxMaterial = new Material;
		boxMaterial->kd = vec3(0.8f, 0.6f, 0.4f);
		boxMaterial->ks = vec3(0.5f, 0.5f, 0.5f);
		boxMaterial->ka = vec3(0.2f, 0.2f, 0.2f);
		boxMaterial->shininess = 1000000;

		Texture* horLinTex = new HorizontalLinearTexture(30, 50);
		Texture* verLinTex = new VerticalLinearTexture(15, 15);
		Texture* squaredTex = new SquaredTexture(30, 50);
		Texture* plainTex = new PlainTexture();

		Geometry* sphere = new Sphere();

		Object* box = new Object(phongShader, boxMaterial, squaredTex, sphere);
		vec3 boxTrans = vec3(0, 0, 2.5);
		vec3 boxRotAxis = vec3(0, 0, 1);
		vec3 boxScale = vec3(6, 6, 6);
		box->transform(boxScale, boxTrans, 0, boxRotAxis);
		objects.push_back(box);

		virus = new Virus(phongShader, material0, verLinTex, material0, horLinTex);
		objects.push_back(virus);

		antibody = new Antibody(phongShader, material1, plainTex);
		objects.push_back(antibody);

		vec3 La = vec3(0.1f, 0.1f, 0.1f);

		lights.resize(3);
		lights[0].wLightPos = vec4(5, 5, 5, 0);
		lights[0].La = La;
		lights[0].Le = vec3(3.0f, 3.0f, 3.0f);

		lights[1].wLightPos = vec4(5, 5, -5, 0);
		lights[1].La = La;
		lights[1].Le = vec3(0.4f, 0.4f, 0.4f);

		lights[2].wLightPos = vec4(-5, 5, 5, 0);
		lights[2].La = La;
		lights[2].Le = vec3(0.1f, 0.1f, 0.1f);

	}

	void Render() {
		RenderState state;
		state.wEye = camera.wEye;
		state.V = camera.V();
		state.P = camera.P();
		state.lights = lights;
		for (Object* obj : objects) obj->Draw(state);
	}

	void Animate(float tstart, float tend) {
		float virusAntibodyDistance = length(virus->getOrigin() - antibody->getOrigin());
		if (virusAntibodyDistance <= virus->getHitRadius() + antibody->getHitRadius())
			virus->kill();

		for (Object* obj : objects) obj->Animate(tstart, tend);
	}

	void onKeyboard(char key) {
		antibody->onKeyboard(key);
	}

	void onKeyboardUp(char key) {
		antibody->onKeyboardUp(key);
	}
};