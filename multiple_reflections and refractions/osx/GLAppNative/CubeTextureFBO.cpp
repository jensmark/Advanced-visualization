#include "CubeTextureFBO.h"
#include "GLUtils/GLUtils.hpp"


CubeTextureFBO::CubeTextureFBO(unsigned int size, int format, GLuint targets, bool storeDepth) {
	this->size = size;
    this->store_depths = storeDepth;

    const GLenum faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
    
    texture.resize(targets);
    
	// Initialize Texture
	glGenTextures(targets, &texture[0]);
    for (size_t i = 0; i < targets; i++) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture[i]);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        for (size_t j = 0; j < 6; j++) {
            glTexImage2D(faces[j], 0, format, size, size, 0, GL_RGBA, GL_FLOAT, NULL);
        }
    }
	
    

    if(!storeDepth){
        //Create depth bufferGLuint rboId;
        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER_EXT, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }else{
        glGenTextures(targets, &depth);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        for (size_t j = 0; j < 6; j++) {
            glTexImage2D(faces[j], 0, GL_DEPTH_COMPONENT24, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
    }

	// Create FBO and attach buffers
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (uint i = 0; i < targets; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, texture[i], 0);
    }
	if (!storeDepth) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
    }else{
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, depth, 0);
    }
    
	
	CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

CubeTextureFBO::~CubeTextureFBO() {
	glDeleteFramebuffersEXT(1, &fbo);
}

void CubeTextureFBO::bind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
}

void CubeTextureFBO::setTarget(GLuint cubemap_side){
    for (size_t i = 0; i < texture.size(); i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(uint)i, cubemap_side, texture[i], 0);
    }
	if (store_depths) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap_side, depth, 0);
    }
    CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
}

void CubeTextureFBO::unbind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}