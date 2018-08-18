#pragma once

#include "Basics.h"
#include "Color.h"
#include "Qt.h"

#define CLASS_GLTHING(Thing, NewThing, DeleteThing) \
class Thing { \
private: \
	GLuint m_obj; \
public: \
	inline Thing() { \
		m_obj = 0; \
	} \
	inline Thing(const Thing&) = delete; \
	inline Thing(Thing &&other) { \
		m_obj = other.m_obj; \
		other.m_obj = 0; \
	} \
	inline ~Thing() { \
		Delete(); \
	} \
	inline Thing& operator=(const Thing&) = delete; \
	inline Thing& operator=(Thing &&other) { \
		if(this != &other) { \
			Delete(); \
			m_obj = other.m_obj; \
			other.m_obj = 0; \
		} \
		return *this; \
	} \
	inline void New() { \
		Delete(); \
		QOpenGLFunctions_3_2_Core *glf = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>(); \
		assert(glf != NULL); \
		NewThing; \
	} \
	inline void Delete() { \
		if(m_obj != 0) { \
			QOpenGLFunctions_3_2_Core *glf = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_2_Core>(); \
			assert(glf != NULL); \
			DeleteThing; \
			m_obj = 0; \
		} \
	} \
	inline operator GLuint() { return m_obj; } \
}

CLASS_GLTHING(GLvbo, glf->glGenBuffers(1, &m_obj), glf->glDeleteBuffers(1, &m_obj));
CLASS_GLTHING(GLvao, glf->glGenVertexArrays(1, &m_obj), glf->glDeleteVertexArrays(1, &m_obj));
CLASS_GLTHING(GLfbo, glf->glGenFramebuffers(1, &m_obj), glf->glDeleteFramebuffers(1, &m_obj));
CLASS_GLTHING(GLtex, glf->glGenTextures(1, &m_obj), glf->glDeleteTextures(1, &m_obj));
CLASS_GLTHING(GLvs, m_obj = glf->glCreateShader(GL_VERTEX_SHADER), glf->glDeleteShader(m_obj));
CLASS_GLTHING(GLgs, m_obj = glf->glCreateShader(GL_GEOMETRY_SHADER), glf->glDeleteShader(m_obj));
CLASS_GLTHING(GLfs, m_obj = glf->glCreateShader(GL_FRAGMENT_SHADER), glf->glDeleteShader(m_obj));
CLASS_GLTHING(GLsp, m_obj = glf->glCreateProgram(), glf->glDeleteProgram(m_obj));

void CompileVS(GLvs &vs, const char *source);
void CompileGS(GLgs &gs, const char *source);
void CompileFS(GLfs &fs, const char *source);
void LinkSP(GLsp& sp, GLuint vs, GLuint gs, GLuint fs,
			std::initializer_list<const char*> attrib_list,
			std::initializer_list<const char*> fragdata_list,
			std::initializer_list<std::pair<GLint&, const char*>> uni_list);

void GLInit();
void GLRegisterDebugCallback();
