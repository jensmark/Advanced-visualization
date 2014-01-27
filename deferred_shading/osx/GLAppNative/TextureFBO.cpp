#include "TextureFBO.h"
#include "GLUtils/GLUtils.hpp"


TextureFBO::TextureFBO(unsigned int width, unsigned int height, int targets, bool storeDepth, int format) {
	this->width = width;
	this->height = height;
    texture.resize(targets);
   
	// Initialize Texture
    glGenTextures(targets, &texture[0]);
    for (size_t i = 0; i < targets; i++) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }
    
    if(!storeDepth){
        //Create depth bufferGLuint rboId;
        glGenRenderbuffers(1, &depth);
        glBindRenderbuffer(GL_RENDERBUFFER_EXT, depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }else{
        glGenTextures(1, &depth);
        glBindTexture(GL_TEXTURE_2D, depth);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    CHECK_GL_ERRORS();
	// Create FBO and attach buffers
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    std::vector<GLenum> attachments;
	for (uint i = 0; i < targets; i++) {
        attachments.push_back(GL_COLOR_ATTACHMENT0+i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, texture[i], 0);
    }
    glDrawBuffers(targets, attachments.data());
    
    if (!storeDepth) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
    }else{
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
    }
    
	
	CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TextureFBO::~TextureFBO() {
	glDeleteFramebuffersEXT(1, &fbo);
}

void TextureFBO::bind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
}

void TextureFBO::unbind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}