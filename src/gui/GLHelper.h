#pragma once

#include "Basics.h"
#include "Color.h"

#include <GL/glew.h>

#include <QtGui>
#include <QtOpenGL/QGLFormat>
#include <QtOpenGL/QGLWidget>

#define CLASS_GLTHING(Thing, NewThing, DeleteThing) \
	class Thing { \
	private: \
		GLuint m_obj; \
	public: \
		inline Thing() { m_obj = 0; } \
		inline Thing(const Thing&) = delete; \
		inline Thing(Thing&& other) { m_obj = other.m_obj; other.m_obj = 0; } \
		inline ~Thing() { Delete(); } \
		inline Thing& operator=(const Thing&) = delete; \
		inline Thing& operator=(Thing&& other) { Delete(); m_obj = other.m_obj; other.m_obj = 0; return *this; } \
		inline void New() { Delete(); NewThing; } \
		inline void Delete() { if(m_obj != 0) { DeleteThing; m_obj = 0; } } \
		inline operator GLuint() { return m_obj; } \
	}

CLASS_GLTHING(GLvbo, glGenBuffers(1, &m_obj), glDeleteBuffers(1, &m_obj));
CLASS_GLTHING(GLvao, glGenVertexArrays(1, &m_obj), glDeleteVertexArrays(1, &m_obj));
CLASS_GLTHING(GLfbo, glGenFramebuffers(1, &m_obj), glDeleteFramebuffers(1, &m_obj));
CLASS_GLTHING(GLtex, glGenTextures(1, &m_obj), glDeleteTextures(1, &m_obj));
CLASS_GLTHING(GLvs, m_obj = glCreateShader(GL_VERTEX_SHADER), glDeleteShader(m_obj));
CLASS_GLTHING(GLgs, m_obj = glCreateShader(GL_GEOMETRY_SHADER), glDeleteShader(m_obj));
CLASS_GLTHING(GLfs, m_obj = glCreateShader(GL_FRAGMENT_SHADER), glDeleteShader(m_obj));
CLASS_GLTHING(GLsp, m_obj = glCreateProgram(), glDeleteProgram(m_obj));

struct GLvec2 {
	float x, y;
	inline GLvec2() {}
	inline GLvec2(float x, float y) : x(x), y(y) {}
	inline GLvec2(const Vertex& v) : x(v.x), y(v.y) {}
	inline operator float*() { return &x; }
};

struct GLvec3 {
	float x, y, z;
	inline GLvec3() {}
	inline GLvec3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline operator float*() { return &x; }
};

struct GLvec4 {
	float x, y, z, w;
	inline GLvec4() {}
	inline GLvec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline GLvec4(const Color &color) : x(color.r), y(color.g), z(color.b), w(color.a) {}
	inline operator float*() { return &x; }
};

static_assert(sizeof(GLvec2) == sizeof(float) * 2, "Invalid GLvec2 struct!");
static_assert(sizeof(GLvec3) == sizeof(float) * 3, "Invalid GLvec3 struct!");
static_assert(sizeof(GLvec4) == sizeof(float) * 4, "Invalid GLvec4 struct!");

inline GLvec4 ColorToVec(const QColor &color) {
	return GLvec4(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}
inline GLvec4 ColorToVec(const Color &color) {
	return GLvec4(color.r, color.g, color.b, color.a);
}

std::string AddLineNumbers(const char *source);

void CompileVS(GLvs &vs, const char *source);
void CompileGS(GLgs &gs, const char *source);
void CompileFS(GLfs &fs, const char *source);
void LinkSP(GLsp& sp, GLuint vs, GLuint gs, GLuint fs,
			std::initializer_list<const char*> attrib_list,
			std::initializer_list<const char*> fragdata_list,
			std::initializer_list<std::pair<GLuint&, const char*>> uni_list);

void GLInit();
void GLRegisterDebugCallback();
