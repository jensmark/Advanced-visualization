#include "Model.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(std::string filename, bool invert) {
	std::vector<float> vertex_data, normal_data, color_data;
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	unsigned int load_flags = aiProcessPreset_TargetRealtime_Quality;;

	//if (invert) load_flags |= aiProcess_ConvertToLeftHanded;

	scene = aiImportFile(filename.c_str(), load_flags);
	if (!scene) {
		std::string log = "Unable to load mesh from ";
		log.append(filename);
		throw std::runtime_error(log);
	}

	//Load the model recursively into data
	min_dim = glm::vec3(std::numeric_limits<float>::max());
	max_dim = -glm::vec3(std::numeric_limits<float>::max());
	//std::cout << min_dim.x << ", " << min_dim.y << ", " << min_dim.z << " - "  << max_dim.x << ", " << max_dim.y << ", " << max_dim.z << std::endl;
	loadRecursive(invert, vertex_data, normal_data, color_data, scene, scene->mRootNode, trafo);
	
	n_vertices = (unsigned int)vertex_data.size()/3;
	
	//Find the bounding box
	for (unsigned int i=0; i<n_vertices; ++i) {
		float x = vertex_data[3*i];
		float y = vertex_data[3*i+1];
		float z = vertex_data[3*i+2];
		
		min_dim.x = std::min(min_dim.x, x);
		min_dim.y = std::min(min_dim.y, y);
		min_dim.z = std::min(min_dim.z, z);

		max_dim.x = std::max(max_dim.x, x);
		max_dim.y = std::max(max_dim.y, y);
		max_dim.z = std::max(max_dim.z, z);
	}

	//Translate to center
	glm::vec3 translation = (max_dim - min_dim) / glm::vec3(2.0f) + min_dim;
	glm::vec3 scale_helper = glm::vec3(1.0f)/(max_dim - min_dim);
	glm::vec3 scale = glm::vec3(std::min(scale_helper.x, std::min(scale_helper.y, scale_helper.z)));
	if (invert) scale = -scale;
	
	transform = glm::mat4(1.0);
	transform = glm::scale(transform, scale);
	transform = glm::translate(transform, -translation);

	//Create the VBOs from the data.
	if (fmod((float)n_vertices, 3.0f) < 0.000001f) 
		vertices.reset(new GLUtils::BO<GL_ARRAY_BUFFER>(vertex_data.data(), (unsigned int)vertex_data.size()*sizeof(float)));
	else
		throw std::runtime_error("The number of vertices in the mesh is wrong");
	if (normal_data.size() == 3*n_vertices) 
		normals.reset(new GLUtils::BO<GL_ARRAY_BUFFER>(normal_data.data(), (unsigned int)normal_data.size()*sizeof(float)));
	if (color_data.size() == 4*n_vertices) 
		colors.reset(new GLUtils::BO<GL_ARRAY_BUFFER>(color_data.data(), (unsigned int)color_data.size()*sizeof(float)));
}

Model::~Model() {

}

void Model::loadRecursive(bool invert,
			std::vector<float>& vertex_data, std::vector<float>& normal_data, 
			std::vector<float>& color_data, 
			const aiScene* scene, const aiNode* node, aiMatrix4x4 modelview_matrix) {
	//update transform matrix. notice that we also transpose it
	aiMultiplyMatrix4(&modelview_matrix, &node->mTransformation);
	aiMatrix4x4 normal_matrix = modelview_matrix.Inverse().Transpose();

	// draw all meshes assigned to this node
	for (unsigned int n=0; n < node->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];

		unsigned int count = mesh->mNumFaces*3;

		//Allocate data
		vertex_data.reserve(vertex_data.size() + count*3);
		if (mesh->HasNormals()) 
			normal_data.reserve(normal_data.size() + count*3);
		if (mesh->mColors[0] != NULL) 
 			color_data.reserve(color_data.size() + count*4);

		//Add the vertices from file
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			//GLenum face_mode;
			
			if(face->mNumIndices != 3) {
				std::cout << "Vertex count for face was " << face->mNumIndices << ", expected 3. Skipping face" << std::endl;
				continue;
				//throw std::runtime_error("Only triangle meshes are supported");
			}

			for(unsigned int i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				aiVector3D tmp = mesh->mVertices[index];
				aiTransformVecByMatrix4(&tmp, &modelview_matrix);

				vertex_data.push_back(tmp.x);
				vertex_data.push_back(tmp.y);
				vertex_data.push_back(tmp.z);
				
				if (mesh->HasNormals()) {
					tmp = mesh->mNormals[index];
					aiTransformVecByMatrix4(&tmp, &normal_matrix);

					if (!invert) {
						normal_data.push_back(tmp.x);
						normal_data.push_back(tmp.y);
						normal_data.push_back(tmp.z);
					}
					else {
						normal_data.push_back(-tmp.x);
						normal_data.push_back(-tmp.y);
						normal_data.push_back(-tmp.z);
					}
				}
				
				if (mesh->mColors[0] != NULL) {
					color_data.push_back(mesh->mColors[0][index].r);
					color_data.push_back(mesh->mColors[0][index].g);
					color_data.push_back(mesh->mColors[0][index].b);
					color_data.push_back(mesh->mColors[0][index].a);
				}
			}
		}
	}

	// load all children
	for (unsigned int n = 0; n < node->mNumChildren; ++n)
		loadRecursive(invert, vertex_data, normal_data, color_data, scene, node->mChildren[n], modelview_matrix);
}