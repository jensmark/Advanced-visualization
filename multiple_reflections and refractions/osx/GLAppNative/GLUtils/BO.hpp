#ifndef _BO_HPP__
#define _BO_HPP__

#include <GL/glew.h>

namespace GLUtils {

template <GLenum T>
class BO {
public:
	BO(const void* data, unsigned int bytes, int usage=GL_STATIC_DRAW) {
		glGenBuffers(1, &vbo_name);
		bind();
		glBufferData(T, bytes, data, usage);
		unbind();
	}

	~BO() {
		unbind();
		glDeleteBuffers(1, &vbo_name);
	}

	inline void bind() {
		glBindBuffer(T, vbo_name);
	}

	static inline void unbind() {
		glBindBuffer(T, 0);
	}

	inline GLuint name() {
		return vbo_name;
	}

private:
	BO() {}
	GLuint vbo_name; //< VBO name
};

};//namespace GLUtils

#endif