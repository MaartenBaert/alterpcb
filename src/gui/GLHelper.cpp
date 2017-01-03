#include "GLHelper.h"

#include <iomanip>
#include <iostream>
#include <sstream>

// Set to 1 to enable OpenGL debugging.
#ifdef NDEBUG
#define GL_ENABLE_DEBUG_CALLBACK 0
#else
#define GL_ENABLE_DEBUG_CALLBACK 1
#endif

// Set to 1 to raise SIGINT when an OpenGL error occurs.
#define GL_SIGINT_ON_ERROR 1

#if GL_ENABLE_DEBUG_CALLBACK && GL_SIGINT_ON_ERROR
#include <csignal>
#endif

std::string AddLineNumbers(const char *source) {
	std::ostringstream ss;
	uint32_t line = 0;
	while(*source != '\0') {
		if(*source == '\n')
			ss << '\n' << std::setw(4) << ++line << '.' << ' ';
		else
			ss << *source;
		++source;
	}
	return ss.str();
}

void CompileVS(GLvs &vs, const char *source) {
	int temp;
	vs.New();
	glShaderSource(vs, 1, &source, 0);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &temp);
	if(temp == GL_FALSE) {
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &temp);
		std::vector<char> log(temp);
		glGetShaderInfoLog(vs, temp, &temp, log.data());
		throw std::runtime_error("Vertex shader compilation failed.\n\nLog:\n" + std::string(log.data(), log.size())
								 + "\n\nSource:\n" + AddLineNumbers(source));
	}
}
void CompileGS(GLgs &gs, const char *source) {
	int temp;
	gs.New();
	glShaderSource(gs, 1, &source, 0);
	glCompileShader(gs);
	glGetShaderiv(gs, GL_COMPILE_STATUS, &temp);
	if(temp == GL_FALSE) {
		glGetShaderiv(gs, GL_INFO_LOG_LENGTH, &temp);
		std::vector<char> log(temp);
		glGetShaderInfoLog(gs, temp, &temp, log.data());
		throw std::runtime_error("Geometry shader compilation failed.\n\nLog:\n" + std::string(log.data(), log.size())
								 + "\n\nSource:\n" + AddLineNumbers(source));
	}
}
void CompileFS(GLfs &fs, const char *source) {
	int temp;
	fs.New();
	glShaderSource(fs, 1, &source, 0);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &temp);
	if(temp == GL_FALSE) {
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &temp);
		std::vector<char> log(temp);
		glGetShaderInfoLog(fs, temp, &temp, log.data());
		throw std::runtime_error("Fragment shader compilation failed.\n\nLog:\n" + std::string(log.data(), log.size())
								 + "\n\nSource:\n" + AddLineNumbers(source));
	}
}
void LinkSP(GLsp& sp, GLuint vs, GLuint gs, GLuint fs,
			std::initializer_list<const char*> attrib_list,
			std::initializer_list<const char*> fragdata_list,
			std::initializer_list<std::pair<GLuint&, const char*>> uni_list) {
	int temp;
	sp.New();
	if(vs != 0)
		glAttachShader(sp, vs);
	if(gs != 0)
		glAttachShader(sp, gs);
	if(fs != 0)
		glAttachShader(sp, fs);
	temp = 0;
	for(auto attrib : attrib_list)
		glBindAttribLocation(sp, temp++, attrib);
	temp = 0;
	for(auto fragdata : fragdata_list)
		glBindFragDataLocation(sp, temp++, fragdata);
	glLinkProgram(sp);
	glGetProgramiv(sp, GL_LINK_STATUS, &temp);
	if(temp == GL_FALSE) {
		glGetProgramiv(sp, GL_INFO_LOG_LENGTH, &temp);
		std::vector<char> log(temp);
		glGetProgramInfoLog(sp, temp, &temp, log.data());
		throw std::runtime_error("Shader program linking failed.\n\nLog:\n" + std::string(log.data(), log.size()));
	}
	for(auto p : uni_list) {
		p.first = glGetUniformLocation(sp, p.second);
	}
}

void GLInit() {

	// print what we can print already
	std::cerr << "[OpenGL] OpenGL version: " << (const char*) glGetString(GL_VERSION) << std::endl;
	std::cerr << "[OpenGL] OpenGL vendor: " << (const char*) glGetString(GL_VENDOR) << std::endl;
	std::cerr << "[OpenGL] OpenGL renderer: " << (const char*) glGetString(GL_RENDERER) << std::endl;
	std::cerr << "[OpenGL] OpenGL extensions: " << (const char*) glGetString(GL_EXTENSIONS) << std::endl;
	std::cerr << "[OpenGL] GLEW version: " << (const char*) glewGetString(GLEW_VERSION) << std::endl;

	// initialize GLEW
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		throw std::runtime_error(std::string("GLEW error: ") + (const char*) glewGetErrorString(err));
	}

}

#if GL_ENABLE_DEBUG_CALLBACK
void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	Q_UNUSED(source); Q_UNUSED(id); Q_UNUSED(userParam);
	if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;
	std::cerr << "[OpenGL] Debug: ";
	switch(type) {
		case GL_DEBUG_TYPE_ERROR:               std::cerr << "ERROR";               break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "DEPRECATED_BEHAVIOR"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "UNDEFINED_BEHAVIOR";  break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "PORTABILITY";         break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "PERFORMANCE";         break;
		case GL_DEBUG_TYPE_OTHER:               std::cerr << "OTHER";               break;
	}
	std::cerr << " / ";
	switch(severity) {
		case GL_DEBUG_SEVERITY_LOW:          std::cerr << "LOW";    break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "MEDIUM"; break;
		case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "HIGH";   break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "NOTIFICATION";   break;
	}
	std::cerr << " / "<< std::string(message, length) << std::endl;
#if GL_SIGINT_ON_ERROR
	if(type == GL_DEBUG_TYPE_ERROR) {
		raise(SIGINT);
	}
#endif
}
#endif

void GLRegisterDebugCallback() {
#if GL_ENABLE_DEBUG_CALLBACK
	GLuint ids = 0;
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &ids, GL_TRUE);
	glDebugMessageCallback(OpenGLDebugCallback, NULL);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_DEBUG_OUTPUT);
	std::cerr << "[OpenGL] Debugging enabled." << std::endl;
#endif
}
