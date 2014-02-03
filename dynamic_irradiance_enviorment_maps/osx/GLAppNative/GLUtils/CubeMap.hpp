#ifndef CUBEMAP_H__
#define CUBEMAP_H__

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>

#include "GLUtils.hpp"

namespace GLUtils {

class CubeMap {
public:
	CubeMap(std::string base_filename, std::string extension) {
		//Load cubemap from file
		load(base_filename, extension);
		CHECK_GL_ERRORS();
	}

	~CubeMap() {};

	void bindTexture(GLenum texture_unit=GL_TEXTURE0) {
		glActiveTexture(texture_unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	}

	static void unbindTexture() {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

private:
	inline void load(std::string base_filename, std::string extension) {
		const char name_exts[6][5] = {"posx", "negx", "posy", "negy", "posz", "negz"};
		const GLenum faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
		std::vector<float> data;
        
		//Allocate texture name and set parameters
		glGenTextures(1, &cubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
		//Load each face and set texture
		for (int i=0; i<6; ++i) {
			std::stringstream filename;
			ILuint ImageName;
			unsigned int width, height;
            
			ilGenImages(1, &ImageName); // Grab a new image name.
			ilBindImage(ImageName); 

			filename << base_filename << name_exts[i] << "." << extension;
            
			if (!ilLoadImage(filename.str().c_str())) {
				ILenum e;
				std::stringstream error;
				while ((e = ilGetError()) != IL_NO_ERROR) {
					error << e << ": " << iluErrorString(e) << std::endl;
				}
				ilDeleteImages(1, &ImageName); // Delete the image name. 
				throw std::runtime_error(error.str());
			}
            
			width = ilGetInteger(IL_IMAGE_WIDTH); // getting image width
			height = ilGetInteger(IL_IMAGE_HEIGHT); // and height
			data.resize(width*height*3);CHECK_GL_ERRORS();
			
			ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data.data());
			ilDeleteImages(1, &ImageName); // Delete the image name. 
            
			glTexImage2D(faces[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
            
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	GLuint cubemap;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > vertices;
	std::shared_ptr<GLUtils::BO<GL_ELEMENT_ARRAY_BUFFER> > indices;
	GLuint vao; //< Vertex array object
	std::shared_ptr<GLUtils::Program> cubemap_program; 
};

} //Namespace

#endif