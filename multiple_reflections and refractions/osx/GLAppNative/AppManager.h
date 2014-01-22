//
//  AppManager.h
//  GLAppNative
//
//  Created by Jens Kristoffer Reitan Markussen on 28.12.13.
//  Copyright (c) 2013 Jens Kristoffer Reitan Markussen. All rights reserved.
//

#ifndef GLAppNative_AppManager_h
#define GLAppNative_AppManager_h

#include "GLUtils/GLUtils.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "Timer.hpp"
#include "CubeTextureFBO.h"
#include "VirtualTrackball.h"
#include "Model.h"

#include "GLUtils/CubeMap.hpp"

using namespace GLUtils;

class AppManager{
public:
    /**
	 * Constructor
	 */
	AppManager();
    
	/**
	 * Destructor
	 */
	~AppManager();
    
	/**
	 * Initializes the game, including the OpenGL context
	 * and data required
	 */
	void init();
    
	/**
	 * The main loop of the app. Runs the main loop
	 */
	void begin();
    
private:
    /**
	 * Quit function
	 */
	void quit();
    
    /**
     * Builds layered distance maps using depth peeling passes
     */
    void buildDistanceMaps(int ref_point);
    
    /**
	 * Function that handles rendering the scene once
	 */
    void renderScene(glm::mat4 view_matrix, glm::mat4 proj_matrix, Program* shader);
    
	/**
	 * Function that handles rendering into the OpenGL context
	 */
	void render();
    
	/**
	 * Creates the OpenGL context using GLFW
	 */
	void createOpenGLContext();
    
	/**
	 * Sets states for OpenGL that we want to keep persistent
	 * throughout the game
	 */
	void setOpenGLStates();
    
	/**
	 * Compiles, attaches, links, and sets uniforms for
	 * a simple OpenGL program
	 */
	void createProgram();
    
	/**
	 * Creates vertex array objects
	 */
	void createVAO();
    
	/**
     * Sets up the FBO for us
     */
	void createFBO();
    
    /**
     * Callback for GLFW key press
     */
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    /**
     * Callback for GLFW mouse button press
     */
    static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
    
    /**
     * Callback for GLFW cursor motion
     */
    static void cursor_callback(GLFWwindow* window, double x, double y);
    
    /**
     * Callback for GLFW error
     */
    static void error_callback(int error, const char* description) {
        THROW_EXCEPTION(description);
    }
    
private:
    static const unsigned int window_width  = 800;
	static const unsigned int window_height = 600;
    
    // GLFW window handle
    GLFWwindow* window;
    
    // Timer
    Timer timer;
    static VirtualTrackball trackball;
    
    // cube
    BO<GL_ARRAY_BUFFER>* vert;
    BO<GL_ARRAY_BUFFER>* nor;
    
    // background;
    CubeMap* cubemap;
    
    // Model to render
    Model* model;
    GLuint vao[2];
    
    // Programs
    Program* phong;
    Program* bg;
    Program* test;
    
    // FBOs
    CubeTextureFBO* distanceMap[3][8]; // 4 layers times 2 depth peeling passes
    
    // Model transformations
    glm::mat4 trans[3];
    
    struct {
		glm::vec3 position;
		glm::mat4 projection;
		glm::mat4 view;
	} light;
    
	struct {
		glm::mat4 projection;
		glm::mat4 view;
	} camera;
};

#endif
