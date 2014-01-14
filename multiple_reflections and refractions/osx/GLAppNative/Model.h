#ifndef _MODEL_H__
#define _MODEL_H__

#include <memory>
#include <string>
#include <vector>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLUtils/BO.hpp"

class Model {
public:
	Model(std::string filename, bool invert=0);
	~Model();

	inline unsigned int getNVertices() {return n_vertices;}
	inline glm::mat4 getTransform() {return transform;}

	inline std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > getVertices() {return vertices;}
	inline std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > getNormals() {return normals;}
	inline std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > getColors() {return colors;}

private:
	static void loadRecursive(bool invert,
			std::vector<float>& vertex_data, std::vector<float>& normal_data, 
			std::vector<float>& color_data,
			const aiScene* scene, const aiNode* node, aiMatrix4x4 modelview_matrix);
			
	const aiScene* scene;

	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > normals;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > vertices;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > colors;

	glm::vec3 min_dim;
	glm::vec3 max_dim;
	glm::mat4 transform;

	unsigned int n_vertices;
};

#endif