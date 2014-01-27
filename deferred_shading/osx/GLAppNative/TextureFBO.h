#ifndef _TEXTUREFBO_HPP__
#define _TEXTUREFBO_HPP__

#include "GLUtils/GLUtils.hpp"
#include <vector>

class TextureFBO {
public:
	TextureFBO(unsigned int width, unsigned int height, int targets = 1, bool storeDepth = false, int format = GL_RGBA32F);
	~TextureFBO();

	void bind();
	static void unbind();

	unsigned int getWidth() {return width; }
	unsigned int getHeight() {return height; }

	GLuint getTexture(size_t i = 0) { return texture[i]; }
    GLuint getDepth(){return depth;}

private:
	GLuint fbo;
	GLuint depth;
    std::vector<GLuint> texture;
	unsigned int width, height;
};

#endif