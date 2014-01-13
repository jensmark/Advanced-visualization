#ifndef _PROGRAM_HPP__
#define _PROGRAM_HPP__

#include "GLException.h"

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

#include <GL/glew.h>

namespace GLUtils {

	
inline std::string readFile(std::string file) {
	long length;
	std::string buffer;
	std::string contents;

	std::ifstream is;
	is.open(file.c_str());

	if (!is.good()) {
		std::string err = "Could not open ";
		err.append(file);
		THROW_EXCEPTION(err);
	}

	// get length of file:
	is.seekg (0, std::ios::end);
	length = is.tellg();
	is.seekg (0, std::ios::beg);

	// reserve memory:
	contents.reserve(length);

	// read data
	while(getline(is,buffer)) {
		contents.append(buffer);
		contents.append("\n");
	}
	is.close();

	return contents;
}



class Program {
public:
	Program(std::string vs, std::string fs) {
		name = glCreateProgram();

		std::string vs_src = readFile(vs);
		std::string fs_src = readFile(fs);

		attachShader(vs_src, GL_VERTEX_SHADER);
		attachShader(fs_src, GL_FRAGMENT_SHADER);
		link();
	}

	Program(std::string vs, std::string gs, std::string fs) {
		name = glCreateProgram();
		std::string vs_src = readFile(vs);
		std::string gs_src = readFile(gs);
		std::string fs_src = readFile(fs);
		
		attachShader(vs_src, GL_VERTEX_SHADER);
		attachShader(gs_src, GL_GEOMETRY_SHADER);
		attachShader(fs_src, GL_FRAGMENT_SHADER);
		link();
	}

	inline void use() {
		glUseProgram(name);
	}

	static inline void disuse() {
		glUseProgram(0);
	}

	inline GLint getUniform(std::string var) {
		GLint loc = glGetUniformLocation(name, var.c_str());
		//assert(loc >= 0);
		return loc;
	}

	inline void setAttributePointer(std::string var, unsigned int size, GLenum type=GL_FLOAT, GLboolean normalized=GL_FALSE, GLsizei stride=0, GLvoid* pointer=NULL) {
		GLint loc = glGetAttribLocation(name, var.c_str());
		assert(loc >= 0);
		glVertexAttribPointer(loc, size, type, normalized, stride, pointer);
		glEnableVertexAttribArray(loc);
	}

private:
	void link() {
		std::stringstream log;
		glLinkProgram(name);

		// check for errors
		GLint linkstatus;
		glGetProgramiv(name, GL_LINK_STATUS, &linkstatus);
		if (linkstatus != GL_TRUE) {
			log << "Linking failed!" << std::endl;

			GLint logsize;
			glGetProgramiv(name, GL_INFO_LOG_LENGTH, &logsize);

			if (logsize > 0) {
				std::vector < GLchar > infolog(logsize + 1);
				glGetProgramInfoLog(name, logsize, NULL, &infolog[0]);
				log << "--- error log ---" << std::endl;
				log << std::string(infolog.begin(), infolog.end()) << std::endl;
			} else {
				log << "--- empty log message ---" << std::endl;
			}
			THROW_EXCEPTION(log.str());
		}
	}

	void attachShader(std::string& src, unsigned int type) {
		std::stringstream log;
		// create shader object
		GLuint s = glCreateShader(type);
		if (s == 0) {
			log << "Failed to create shader of type " << type << std::endl;
			THROW_EXCEPTION(log.str());
		}

		// set source code and compile
		const GLchar* src_list[1] = { src.c_str() };
		glShaderSource(s, 1, src_list, NULL);
		glCompileShader(s);

		// check for errors
		GLint compile_status;
		glGetShaderiv(s, GL_COMPILE_STATUS, &compile_status);
		if (compile_status != GL_TRUE) {
			// compilation failed
			log << "Compilation failed!" << std::endl;
			log << "--- source code ---" << std::endl;
			std::istringstream src_ss(src);
			std::string line;
			unsigned int i=0;
			while (std::getline(src_ss, line))
				log << std::setw(4) << std::setfill('0') << ++i << line << std::endl;

			GLint logsize;
			glGetShaderiv(s, GL_INFO_LOG_LENGTH, &logsize);
			if (logsize > 0) {
				std::vector<GLchar> infolog(logsize + 1);
				glGetShaderInfoLog(s, logsize, NULL, &infolog[0]);

				log << "--- error log ---" << std::endl;
				log << std::string(infolog.begin(), infolog.end()) << std::endl;
			}
			else {
				log << "--- empty log message ---" << std::endl;
			}
			THROW_EXCEPTION(log.str());
		}
		
		glAttachShader(name, s);
	}

	GLuint name; //< OpenGL shader program

};

}; //Namespace GLUtils

#endif