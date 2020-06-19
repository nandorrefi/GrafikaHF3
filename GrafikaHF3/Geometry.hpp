#pragma once
#include "RenderState.hpp"

template<class T> struct Dnum {
	float f;
	T d;
	Dnum(float f0 = 0, T d0 = T(0)) { f = f0, d = d0; }
	Dnum operator+(Dnum r) { return Dnum(f + r.f, d + r.d); }
	Dnum operator-(Dnum r) { return Dnum(f - r.f, d - r.d); }
	Dnum operator*(Dnum r) { return Dnum(f * r.f, f * r.d + d * r.f); }
	Dnum operator/(Dnum r) {
		float l = r.f * r.f;
		return (*this) * Dnum(r.f / l, -r.d / l);
	}
};

template<class T> Dnum<T> Sin(Dnum<T> g) { return Dnum<T>(sinf(g.f), cosf(g.f) * g.d); }
template<class T> Dnum<T> Cos(Dnum<T> g) { return Dnum<T>(cosf(g.f), -sinf(g.f) * g.d); }
template<class T> Dnum<T> Tan(Dnum<T> g) { return Sin(g) / Cos(g); }
template<class T> Dnum<T> Sinh(Dnum<T> g) { return Dnum<T>(sinhf(g.f), coshf(g.f) * g.d); }
template<class T> Dnum<T> Cosh(Dnum<T> g) { return Dnum<T>(coshf(g.f), -sinhf(g.f) * g.d); }
template<class T> Dnum<T> Tanh(Dnum<T> g) { return Sinh(g) / Cosh(g); }

typedef Dnum<vec2> Dnum2;

class Geometry {
protected:
	unsigned int vao, vbo;  
	float t = 0.0f;
public:
	Geometry() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}
	
	virtual ~Geometry() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	virtual void setT(float time) = 0;

	virtual void Draw() = 0;
};


const int tessellationLevel = 30;

class ParamSurface : public Geometry {
	unsigned int nVtxPerStrip, nStrips;

	int N, M;
public:
	ParamSurface() { 
		nVtxPerStrip = nStrips = 0; 
	}

	virtual ~ParamSurface() {

	}

	virtual void eval(Dnum2& U, Dnum2& V, Dnum2& X, Dnum2& Y, Dnum2& Z) = 0;

	VertexData GenVertexData(float u, float v) {
		VertexData vd;

		Dnum2 U(u, vec2(1, 0));
		Dnum2 V(v, vec2(0, 1));
		Dnum2 X, Y, Z;
		
		eval(U, V, X, Y, Z);

		vd.position = vec3(X.f, Y.f, Z.f);

		vec3 drdU(X.d.x, Y.d.x, Z.d.x);
		vec3 drdV(X.d.y, Y.d.y, Z.d.y);

		vd.normal = cross(drdU, drdV);

		vd.texcoord = vec2(u, v);
		return vd;
	}

	void genAndBufferVertices() {
		nVtxPerStrip = (M + 1) * 2;
		nStrips = N;
		std::vector<VertexData> vtxData;
		for (int i = 0; i < N; i++) {
			for (int j = 0; j <= M; j++) {
				vtxData.push_back(GenVertexData((float)j / M, (float)i / N));
				vtxData.push_back(GenVertexData((float)j / M, (float)(i + 1) / N));
			}
		}
		glBufferData(GL_ARRAY_BUFFER, nVtxPerStrip * nStrips * sizeof(VertexData), &vtxData[0], GL_DYNAMIC_DRAW);
	}

	void create(int N = tessellationLevel, int M = tessellationLevel) {
		this->N = N;
		this->M = M;

		genAndBufferVertices();

		glEnableVertexAttribArray(0);  
		glEnableVertexAttribArray(1);  
		glEnableVertexAttribArray(2); 

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texcoord));

	}

	void setT(float time) {
		t = time;
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		genAndBufferVertices();
	}

	void Draw() {
		glBindVertexArray(vao);
		for (unsigned int i = 0; i < nStrips; i++) glDrawArrays(GL_TRIANGLE_STRIP, i * nVtxPerStrip, nVtxPerStrip);
	}
};