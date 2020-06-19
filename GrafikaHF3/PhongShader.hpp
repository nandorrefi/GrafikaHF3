#pragma once
#include "Shader.hpp"

class PhongShader : public Shader {
	const char* vertexSource = R"(
		#version 330
		#define M_PI 3.1415926535897932384626433832795

		precision highp float;

		struct Light {
			vec3 La, Le;
			vec4 wLightPos;
		};

		uniform mat4  MVP, M, Minv;
		uniform Light[8] lights;    
		uniform int   nLights;
		uniform vec3  wEye;         
		uniform float t;

		layout(location = 0) in vec3  vtxPos;           
		layout(location = 1) in vec3  vtxNorm;      
		layout(location = 2) in vec2  vtxUV;

		out vec3 wNormal;		  
		out vec3 wView;           
		out vec3 wLight[8];		    
		out vec2 texcoord;

		void main() {
			vec3 posT = vtxPos; //* R();		

			gl_Position = vec4(posT, 1) * MVP;
			
			vec4 wPos = vec4(posT, 1) * M;
			for(int i = 0; i < nLights; i++) {
				wLight[i] = lights[i].wLightPos.xyz * wPos.w - wPos.xyz * lights[i].wLightPos.w;
			}
		    wView  = wEye * wPos.w - wPos.xyz;

		    wNormal = (Minv * vec4(vtxNorm, 0)).xyz;
		    texcoord = vtxUV;
		}
	)";


	const char* fragmentSource = R"(
		#version 330
		precision highp float;

		struct Light {
			vec3 La, Le;
			vec4 wLightPos;
		};

		struct Material {
			vec3 kd, ks, ka;
			float shininess;
		};

		uniform Material material;
		uniform Light[8] lights;  
		uniform int   nLights;
		uniform sampler2D diffuseTexture;

		in  vec3 wNormal;  
		in  vec3 wView;   
		in  vec3 wLight[8]; 
		in  vec2 texcoord;
		
        out vec4 fragmentColor; 

		void main() {
			vec3 N = normalize(wNormal);
			vec3 V = normalize(wView); 
			if (dot(N, V) < 0) N = -N;
			vec3 texColor = texture(diffuseTexture, texcoord).rgb;
			vec3 ka = material.ka * texColor;
			vec3 kd = material.kd * texColor;

			vec3 radiance = vec3(0, 0, 0);
			for(int i = 0; i < nLights; i++) {
				vec3 L = normalize(wLight[i]);
				float cost = max(dot(N,L), 0);			

				vec3 H = normalize(L + V);
				float cosd = max(dot(N,H), 0);

				radiance += ka * lights[i].La + (kd * cost + material.ks * pow(cosd, material.shininess)) * lights[i].Le;
			}
			fragmentColor = vec4(radiance, 1);
		}
	)";
public:
	PhongShader() { create(vertexSource, fragmentSource, "fragmentColor"); }

	void Bind(RenderState& state) {
		Use(); 
		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.Minv, "Minv");
		setUniform(state.wEye, "wEye");
		setUniform(*state.texture, std::string("diffuseTexture"));
		setUniformMaterial(*state.material, "material");

		setUniform((int)state.lights.size(), "nLights");
		for (unsigned int i = 0; i < state.lights.size(); i++) {
			setUniformLight(state.lights[i], std::string("lights[") + std::to_string(i) + std::string("]"));
		}
	}
};