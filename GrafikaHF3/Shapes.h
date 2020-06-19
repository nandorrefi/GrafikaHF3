#pragma once
#include "Geometry.h"



class Sphere : public ParamSurface {
public:
	Sphere() {
		create(80, 80);
	}

	void eval(Dnum2& U, Dnum2& V, Dnum2& X, Dnum2& Y, Dnum2& Z) {
		U = U * 2 * (float)M_PI;
		V = V * (float)M_PI;

		X = Cos(U) * Sin(V);
		Y = Sin(U) * Sin(V);
		Z = Cos(V);
	}

};

class MorphingSphere : public ParamSurface {
public:
	MorphingSphere() { 
		create(80, 80);
	}

	Dnum2 R(Dnum2 U, Dnum2 V) {
		U = U * 4;
		V = V * 10;
		return Sin(U)*Sin(V)*sin(t)/2.5f + 1.0f;
	}

	void eval(Dnum2& U, Dnum2& V, Dnum2& X, Dnum2& Y, Dnum2& Z) {
		U = U * 2 * (float)M_PI;
		V = V * (float)M_PI;

		X = Cos(U) * Sin(V) * R(U, V);
		Y = Sin(U) * Sin(V) * R(U, V);
		Z = Cos(V) * R(U, V);
	}

};

class Tractricoid : public ParamSurface {
public:
	Tractricoid() { create(); }

	void eval(Dnum2& U, Dnum2& V, Dnum2& X, Dnum2& Y, Dnum2& Z) {
		const float height = 3.0f;
		
		U = U * height;
		V = V * 2 * M_PI;

		X = Cos(V) / Cosh(U);
		Y = Sin(V) / Cosh(U);
		Z = U - Tanh(U);
	}

};

vec3 getMidpoint(const vec3& point1, const vec3& point2) {
	return (point1 + point2) / 2;
}

class Triangle {
	std::vector<vec3> positions;
	vec3 normal;
	vec3 centroid;

	void initPositions(const vec3& pos1, const vec3& pos2, const vec3& pos3) {
		positions.push_back(pos1);
		positions.push_back(pos2);
		positions.push_back(pos3);
	}

	void initNormal() {
		vec3 a = positions[1] - positions[0];
		vec3 b = positions[2] - positions[0];
		normal = cross(a, b);
	}

	void initCentroid() {
		vec3 midpoint = getMidpoint(positions[0], positions[1]);
		vec3 median = positions[2] - midpoint;
		centroid = midpoint + median / 3;
	}

public:
	Triangle(const vec3& pos1, const vec3& pos2, const vec3& pos3) {
		initPositions(pos1, pos2, pos3);
		initNormal();
		initCentroid();
	}

	std::vector<VertexData> getVertexData() {
		std::vector<VertexData> vertices;
		
		for (int i = 0; i < positions.size(); ++i) {
			VertexData vertex;
			vertex.position = positions[i];
			vertex.normal = normal;
			vertex.texcoord = vec2(0, 0);

			vertices.push_back(vertex);
		}

		return vertices;
	}

	vec3 getPosition(int i) {
		return positions[i];
	}

	vec3 getNormal() {
		return normal;
	}

	vec3 getCentroid() {
		return centroid;
	}
};

class Tetrahedron: public Geometry {
	std::vector<Triangle> sides;
public:
	Tetrahedron() { 
		vec3 positions[] = { vec3( 1, -1, 1), vec3(1, 1, -1), vec3(-1, 1, 1), vec3(-1, -1, -1) };
		init(positions);
	
	}

	Tetrahedron(vec3 positions[4]) {
		init(positions);
	}

	void init(vec3 positions[4]) {
		sides.push_back(Triangle(positions[0], positions[1], positions[2]));
		sides.push_back(Triangle(positions[0], positions[3], positions[1]));
		sides.push_back(Triangle(positions[2], positions[1], positions[3]));
		sides.push_back(Triangle(positions[2], positions[3], positions[0]));

		std::vector<VertexData> vtxData;

		for (int i = 0; i < sides.size(); ++i) {
			std::vector<VertexData> sideVtxData = sides[i].getVertexData();
			vtxData.insert(vtxData.end(), sideVtxData.begin(), sideVtxData.end());
		}

		glBufferData(GL_ARRAY_BUFFER, vtxData.size() * sizeof(VertexData), &vtxData[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);  
		glEnableVertexAttribArray(1);  
		glEnableVertexAttribArray(2);  

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texcoord));
	}

	void setT(float time) {
		t = time;
	}

	void Draw() {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 4*3);
	}

	Triangle getSide(int i) {
		return sides[i];
	}
	
};