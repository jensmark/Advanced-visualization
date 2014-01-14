#ifndef _TEXTUREFBO_HPP__
#define _TEXTUREFBO_HPP__

#include "GLUtils/GLUtils.hpp"

class TextureFBO {
public:
	TextureFBO(unsigned int width, unsigned int height, int format = GL_RGBA32F, bool storeDepth = false);
	~TextureFBO();

	void bind();
	static void unbind();

	unsigned int getWidth() {return width; }
	unsigned int getHeight() {return height; }

	GLuint getTexture() { return texture; }
    GLuint getDepth() { return depth; }

private:
	GLuint fbo;
	GLuint depth;
	GLuint texture;
	unsigned int width, height;
};

#endif