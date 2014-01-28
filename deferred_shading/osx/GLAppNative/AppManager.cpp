//
//  AppManager.cpp
//  GLAppNative
//
//  Created by Jens Kristoffer Reitan Markussen on 28.12.13.
//  Copyright (c) 2013 Jens Kristoffer Reitan Markussen. All rights reserved.
//

#include "AppManager.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

// Static decleration
VirtualTrackball AppManager::trackball;

AppManager::AppManager(){
}

AppManager::~AppManager(){
}

void AppManager::init(){
    /* Initialize the library */
    if (glfwInit() != GL_TRUE) {
        THROW_EXCEPTION("Failed to initialize GLFW");
    }
    glfwSetErrorCallback(error_callback);
    
    createOpenGLContext();
    setOpenGLStates();
    
    trackball.setWindowSize(window_width, window_height);
    model = new Model("bunny.obj", false);
    sphere = new Model("sphere.obj", false);
    
    camera.projection = glm::perspective(45.0f,
                    window_width / (float) window_height, 1.0f, 50.0f);
	camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
    
    // lights
    light[0].position = glm::vec3(0.0f,0.5f,0.0f);
    light[0].diffuse = glm::vec3(0.0f,1.0f,0.0f);
    light[0].specular = glm::vec3(1.0f,1.0f,1.0f);
    
    light[1].position = glm::vec3(0.5f,0.0f,0.0f);
    light[1].diffuse = glm::vec3(0.0f,0.0f,1.0f);
    light[1].specular = glm::vec3(1.0f,1.0f,1.0f);
    
    light[2].position = glm::vec3(0.0f,0.0f,0.5f);
    light[2].diffuse = glm::vec3(1.0f,0.0f,0.0f);
    light[2].specular = glm::vec3(1.0f,1.0f,1.0f);
    
    createFBO();
    createProgram();
    createVAO();
}

void AppManager::begin(){
    while (!glfwWindowShouldClose(window)) {
        /* Poll for and process events */
        glfwPollEvents();
        
        /* Render loop */
        render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }
    
    /* Clean up everything */
    quit();
}

void AppManager::quit(){
    delete phong;
    delete model;
    delete gBuffer;
    delete vert;
    delete ind;
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void AppManager::renderModel(TextureFBO* target, Program* shader, glm::mat4& proj, glm::mat4& mw, glm::mat3& nor){
    target->bind();
    glViewport(0, 0, target->getWidth(), target->getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader->use();
    glBindVertexArray(vao[0]);
    
    glUniformMatrix4fv(shader->getUniform("projection_matrix"), 1, 0, glm::value_ptr(proj));
    glUniformMatrix4fv(shader->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(mw));
    glUniformMatrix3fv(shader->getUniform("normal_matrix"), 1, 0, glm::value_ptr(nor));
    glUniform4fv(shader->getUniform("color"), 1, glm::value_ptr(glm::vec4(1.0f,1.0f,1.0f,1.0f)));
    
    glDrawArrays(GL_TRIANGLES, 0, model->getNVertices());
    
    glBindVertexArray(0);
    shader->disuse();
    
    target->unbind();
}

void AppManager::render(){
    //Create the new view matrix that takes the trackball view into account
	glm::mat4 view_matrix_new = camera.view*trackball.getTransform();
    glm::mat4 model_view_matrix = view_matrix_new*model->getTransform();
    glm::mat3 normal_matrix = glm::mat3(glm::inverse(glm::transpose(model_view_matrix)));
    
    renderModel(gBuffer, deferred, camera.projection, model_view_matrix, normal_matrix);
    
    glViewport(0, 0, window_width*2, window_height*2);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable (GL_BLEND);
    glBlendEquation (GL_FUNC_ADD);
    glBlendFunc (GL_ONE, GL_ONE);
    
    glDisable(GL_CULL_FACE);
    glDisable (GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);
    
    phong1->use();
    glBindVertexArray(vao[2]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(0));
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(1));

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(2));
    
    for (size_t i = 0; i < 3; i++) {
        glm::mat4 model = glm::translate(sphere->getTransform(), light[i].position);
        glUniformMatrix4fv(phong1->getUniform("projection_matrix"), 1, 0, glm::value_ptr(camera.projection));
        glUniformMatrix4fv(phong1->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(view_matrix_new*model));
        glUniformMatrix4fv(phong1->getUniform("view_matrix"), 1, 0, glm::value_ptr(view_matrix_new));
        glUniform3fv(phong1->getUniform("light_pos"), 1, glm::value_ptr(light[i].position));
        
        glUniform3fv(phong1->getUniform("diffuse"), 1, glm::value_ptr(light[i].diffuse));
        glUniform3fv(phong1->getUniform("specular"), 1, glm::value_ptr(light[i].specular));
        
        glDrawArrays(GL_TRIANGLES, 0, sphere->getNVertices());
    }
    
    glBindVertexArray(0);
    phong1->disuse();
    
    glEnable(GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glDisable (GL_BLEND);
    
    CHECK_GL_ERRORS();
}

void AppManager::createOpenGLContext(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "GL App", NULL, NULL);
    if (window == NULL) {
        THROW_EXCEPTION("Failed to create window");
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursor_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::stringstream err;
		err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
		THROW_EXCEPTION(err.str());
	}
    
	// Unfortunately glewInit generates an OpenGL error, but does what it's
	// supposed to (setting function pointers for core functionality).
	// Lets do the ugly thing of swallowing the error....
    glGetError();
}

void AppManager::setOpenGLStates(){
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
    
	glClearColor(0.0, 0.0, 0.0, 1.0);
    
    CHECK_GL_ERRORS();
}

void AppManager::createProgram(){
    //Build programs
    phong = new Program("kernel.vert","phong.frag");
    phong1 = new Program("trans_basic.vert", "phong.frag");
    deferred = new Program("trans.vert","deferred.frag");
    
    //Set uniforms
    phong->use();
    glUniform3fv(phong->getUniform("diffuse"), 1, glm::value_ptr(glm::vec3(0.8f,0.0f,0.0f)));
    glUniform3fv(phong->getUniform("specular"), 1, glm::value_ptr(glm::vec3(1.0f,1.0f,1.0f)));
    glUniform1f(phong->getUniform("shininess"), 120.0f);
    
    glUniform1i(phong->getUniform("color"),0);
    glUniform1i(phong->getUniform("normal"),1);
    glUniform1i(phong->getUniform("view"),2);
    //glUniform1i(phong->getUniform("depth"),3);
    phong->disuse();
    
    phong1->use();
    glUniform3fv(phong1->getUniform("diffuse"), 1, glm::value_ptr(glm::vec3(0.0f,1.0f,0.0f)));
    glUniform3fv(phong1->getUniform("specular"), 1, glm::value_ptr(glm::vec3(1.0f,1.0f,1.0f)));
    glUniform1f(phong1->getUniform("shininess"), 120.0f);
    
    glUniform1i(phong1->getUniform("color"),0);
    glUniform1i(phong1->getUniform("normal"),1);
    glUniform1i(phong1->getUniform("view"),2);
    //glUniform1i(phong1->getUniform("depth"),3);
    phong1->disuse();

     
    CHECK_GL_ERRORS();
}

void AppManager::createVAO(){
    glGenVertexArrays(3, &vao[0]);
    
	glBindVertexArray(vao[0]);
	model->getVertices()->bind();
	deferred->setAttributePointer("position", 3);
	model->getNormals()->bind();
	deferred->setAttributePointer("normal", 3);
	model->getVertices()->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
    
    glBindVertexArray(vao[1]);
    GLfloat quad_vertices[] =  {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
        -1.0f,  1.0f,
    };
    vert = new BO<GL_ARRAY_BUFFER>(quad_vertices, sizeof(quad_vertices));
    
    GLubyte quad_indices[] = {
        0, 1, 2, //triangle 1
        2, 3, 0, //triangle 2
    };
    ind = new BO<GL_ELEMENT_ARRAY_BUFFER>(quad_indices, sizeof(quad_indices));
    vert->bind();
    phong->setAttributePointer("position", 2);
    ind->bind();
    
    glBindVertexArray(0);
    
    glBindVertexArray(vao[2]);
	sphere->getVertices()->bind();
	phong1->setAttributePointer("position", 3);
	sphere->getVertices()->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
    
    CHECK_GL_ERRORS();
}

void AppManager::createFBO(){
    gBuffer = new TextureFBO(window_width, window_height, 3, true);
    
    CHECK_GL_ERRORS();
}

void AppManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void AppManager::mouse_callback(GLFWwindow* window, int button, int action, int mods){
    double x,y;
    glfwGetCursorPos(window, &x, &y);
    
    if (action == GLFW_PRESS) {
        trackball.rotateBegin((int)x, (int)y);
    } else {
        trackball.rotateEnd((int)x, (int)y);
    }
}

void AppManager::cursor_callback(GLFWwindow* window, double x, double y){
    trackball.rotate(x,y);
}