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

#include <IL/il.h>
#include <IL/ilu.h>

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
    
    ilInit();
    iluInit();
    
    createOpenGLContext();
    setOpenGLStates();
    
    trackball.setWindowSize(window_width, window_height);
    model = new Model("sphere.obj", false);
    
    srand((unsigned)time(0));
   
    for (size_t i = 0; i < 3; i++) {
        float tx = (rand() / (float) RAND_MAX - 0.5f) * 8.0f;
		float ty = (rand() / (float) RAND_MAX - 0.5f) * 8.0f;
		float tz = (rand() / (float) RAND_MAX - 0.5f) * 8.0f;
        
        std::cout << tx << " " << ty << " " << tz << std::endl;
        
        trans[i] = glm::translate(model->getTransform(), glm::vec3(tx,ty,tz));
    }
    
    camera.projection = glm::perspective(45.0f,
                    window_width / (float) window_height, 1.0f, 50.0f);
	camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -6.0f));
    
    light.position = glm::vec3(0.0f,0.0f,-10.0f);
    //light.view = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));
    //light.projection = glm::perspective(45.0f, 1.0f, 1.0f, 50.0f);
    
    cubemap = new CubeMap("","jpg");
    
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
    delete vert;
    delete nor;
    
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 8; j++) {
            delete distanceMap[i][j];
        }
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void AppManager::buildDistanceMaps(int ref_point){
    glm::vec3 point = glm::vec3(trans[ref_point]*glm::vec4(1.0f));
    
    const glm::mat4 proj        = glm::perspective(90.0f, 1.0f, 0.1f, 100.0f);
    const glm::mat4 views[6]    = {
        glm::lookAt(point, glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f)),
        glm::lookAt(point, glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f)),
        glm::lookAt(point, glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f)),
        glm::lookAt(point, glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f)),
        glm::lookAt(point, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,1.0f,0.0f)),
        glm::lookAt(point, glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,1.0f,0.0f))};
    const GLenum faces[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
    
    
    for (size_t i = 0; i < 4; i++) {
        CubeTextureFBO* fboA = distanceMap[ref_point][i];
        //CubeTextureFBO* fboB = distanceMap[ref_point][(i+1)%2];
        
        fboA->bind();
        
        for (size_t j = 0; j < 6; j++) {
            
            fboA->setTarget(faces[j]);
            glViewport(0, 0, fboA->getSize(), fboA->getSize());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            renderScene(views[j], proj, phong);
            
        }
        fboA->unbind();
    }
}

void AppManager::renderScene(glm::mat4 view_matrix, glm::mat4 proj_matrix, Program* shader){
    
    bg->use();
    glBindVertexArray(vao[1]);
    
    glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(15.0f));
    glm::mat4 model_view_matrix = view_matrix*model_matrix;
    glm::mat3 normal_matrix = glm::mat3(glm::inverse(glm::transpose(model_view_matrix)));
    
    glUniformMatrix4fv(bg->getUniform("projection_matrix"), 1, 0, glm::value_ptr(camera.projection));
    glUniformMatrix4fv(bg->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(model_view_matrix));
    glUniformMatrix3fv(bg->getUniform("normal_matrix"), 1, 0, glm::value_ptr(normal_matrix));
    glUniform3fv(bg->getUniform("light_pos"), 1, glm::value_ptr(light.position));
    
    cubemap->bindTexture();
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    bg->disuse();
    
    //int tst = 0;
    
    for (size_t i = 0; i < 3; i++) {
        model_view_matrix = view_matrix*trans[i];
        normal_matrix = glm::mat3(glm::inverse(glm::transpose(model_view_matrix)));
        //if (i == tst){
            shader->use();
        //} else {
        //    phong->use();
        //}
        glBindVertexArray(vao[0]);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, distanceMap[i][0]->getTexture());
        //cubemap->bindTexture();
        
        glUniformMatrix4fv(shader->getUniform("projection_matrix"), 1, 0, glm::value_ptr(camera.projection));
        glUniformMatrix4fv(shader->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(model_view_matrix));
        glUniformMatrix3fv(shader->getUniform("normal_matrix"), 1, 0, glm::value_ptr(normal_matrix));
        glUniform3fv(shader->getUniform("light_pos"), 1, glm::value_ptr(light.position));
        
        glDrawArrays(GL_TRIANGLES, 0, model->getNVertices());
        
        glBindVertexArray(0);
        shader->disuse();

    }
}

void AppManager::render(){
    for (int i = 0; i < 3; i++) {
        buildDistanceMaps(i);
    }
    
    glViewport(0, 0, window_width*2, window_height*2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Create the new view matrix that takes the trackball view into account
	glm::mat4 view_matrix_new = camera.view*trackball.getTransform();
    
    renderScene(view_matrix_new, camera.projection, phong);
    CHECK_GL_ERRORS();
}

void AppManager::createOpenGLContext(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
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
	//glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    
	glClearColor(0.0, 0.5, 0.5, 1.0);
    
    CHECK_GL_ERRORS();
}

void AppManager::createProgram(){
    //Build programs
    phong   = new Program("trans.vert", "phong.frag");
    bg      = new Program("trans.vert", "cubemap.frag");
    test    = new Program("trans.vert", "map_test.frag");
    
    //Set uniforms
    phong->use();
    glUniform3fv(phong->getUniform("diffuse"), 1, glm::value_ptr(glm::vec3(0.8f,0.0f,0.0f)));
    glUniform3fv(phong->getUniform("specular"), 1, glm::value_ptr(glm::vec3(1.0f,1.0f,1.0f)));
    glUniform3fv(phong->getUniform("ambient"), 1, glm::value_ptr(glm::vec3(0.5f,0.0f,0.0f)));
    glUniform1f(phong->getUniform("shininess"), 120.0f);
    phong->disuse();
    
    bg->use();
    glUniform1i(bg->getUniform("tex"), 0);
    bg->disuse();
    
    test->use();
    glUniform1i(test->getUniform("env_map"), 0);
    test->disuse();
    
    CHECK_GL_ERRORS();
}

void AppManager::createVAO(){
    glGenVertexArrays(2, &vao[0]);
    
	glBindVertexArray(vao[0]);
	model->getVertices()->bind();
	phong->setAttributePointer("position", 3);
	model->getNormals()->bind();
	phong->setAttributePointer("normal", 3);
	model->getVertices()->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
    
    glBindVertexArray(vao[1]);
    const float cube_vertices_data[] = {
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
    
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
     
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
     
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
     
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
     
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
    };
    vert = new BO<GL_ARRAY_BUFFER>(&cube_vertices_data[0], sizeof(cube_vertices_data));
    
    const float cube_normals_data[] = {
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f, -1.0f,
         0.0f, 0.0f, -1.0f,
     
         -1.0f, 0.0f, 0.0f,
         -1.0f, 0.0f, 0.0f,
         -1.0f, 0.0f, 0.0f,
         -1.0f, 0.0f, 0.0f,
         -1.0f, 0.0f, 0.0f,
         -1.0f, 0.0f, 0.0f,
     
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f,
     
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
     
         0.0f, -1.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
     
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
    };
    nor = new BO<GL_ARRAY_BUFFER>(&cube_normals_data[0], sizeof(cube_normals_data));
 
    vert->bind();
	phong->setAttributePointer("position", 3);
	nor->bind();
	phong->setAttributePointer("normal", 3);
	model->getVertices()->unbind(); //Unbinds both vertices and normals
    
    glBindVertexArray(0);
    
    CHECK_GL_ERRORS();
}

void AppManager::createFBO(){
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 8; j++) {
            distanceMap[i][j] = new CubeTextureFBO(512, GL_RGBA16F, 1);
        }
    }
    
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