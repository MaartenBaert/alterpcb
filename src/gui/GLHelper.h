#pragma once

#include "Basics.h"
#include "Color.h"
#include "Qt.h"

#define CLASS_GLTHING(Thing, NewThing, DeleteThing) \
	class Thing { \
	private: \
		GLuint m_obj; \
	public: \
		inline Thing() { m_obj = 0; } \
		inline Thing(const Thing&) = delete; \
		inline Thing(Thing &&other) { m_obj = other.m_obj; other.m_obj = 0; } \
		inline ~Thing() { Delete(); } \
		inline Thing& operator=(const Thing&) = delete; \
		inline Thing& operator=(Thing &&other) { if(this != &other) { Delete(); m_obj = other.m_obj; other.m_obj = 0; } return *this; } \
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
