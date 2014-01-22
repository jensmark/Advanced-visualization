#ifndef _TEXTUREFBO_HPP__
#define _TEXTUREFBO_HPP__

#include <vector>
#include "GLUtils/GLUtils.hpp"

class CubeTextureFBO {
public:
	CubeTextureFBO(unsigned int size, int format = GL_RGBA32F, GLuint targets = 1, bool storeDepth = false);
	~CubeTextureFBO();

	void bind();
	static void unbind();

    void setTarget(GLuint cubemap_side);
    
	unsigned int getSize() {return size; }

	GLuint getTexture(GLuint target = 0) { return texture[target]; }
    GLuint getDepth() { return depth; }

private:
	GLuint fbo;
    
    GLuint depth;
    std::vector<GLuint> texture;
	
    unsigned int size;
    bool store_depths;
};

#endif