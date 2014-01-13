#include "VirtualTrackball.h"
#include <cmath>
#include <iostream>
#include <algorithm>

glm::mat4 quatToMat4(glm::quat m_q) {
    /**
     * Implement so that the we generate the correct transformation
     * matrix from the input quaternion
     */
	glm::quat q;
    
	// normalize quaterion if longer then 1.0
	if(glm::length(m_q) > 1.0){
		q = glm::normalize(m_q);
	}else{
		q = m_q;
	}
    
	//glm::mat4 m = glm::toMat4(q);
    
	// create rotation matrix
	glm::mat4 m(1 - (2 * (pow(q.y, 2) + pow(q.z, 2))), 2 * (q.x * q.y - q.w * q.z),   2 * (q.x * q.z + q.w * q.y),   0,
                2 * (q.x * q.y + q.w * q.z),    1 - (2 * (pow(q.x, 2) + pow(q.z, 2))), 2 * (q.y * q.z - q.w * q.x),   0,
                2 * (q.x * q.z - q.w * q.y),    2 * (q.y * q.z + q.w * q.x),   1 - (2 * (pow(q.x, 2) + pow(q.y, 2))), 0,
                0,          0,          0,          1);
    
	return m;
}

VirtualTrackball::VirtualTrackball() {
	view_quat_old.w = 1.0;
	view_quat_old.x = 0.0;
	view_quat_old.y = 0.0;
	view_quat_old.z = 0.0;
	rotating = false;
}

VirtualTrackball::~VirtualTrackball() {}

void VirtualTrackball::rotateBegin(int x, int y) {
	rotating = true;
	point_on_sphere_begin = getClosestPointOnUnitSphere(x, y);
}

void VirtualTrackball::rotateEnd(int x, int y) {
	rotating = false;
	view_quat_old = view_quat_new;
}

void VirtualTrackball::rotate(int x, int y) {
    //If not rotating, simply return the old rotation matrix
	if (!rotating) return;
    
	glm::vec3 point_on_sphere_end; //Current point on unit sphere
	glm::vec3 axis_of_rotation; //axis of rotation
	float theta = 0.0f; //angle of rotation
    
	point_on_sphere_end = getClosestPointOnUnitSphere(x, y);
    
	/**
	 * normalize vectors if needed
	 */
	point_on_sphere_begin = glm::normalize(point_on_sphere_begin);
	point_on_sphere_end = glm::normalize(point_on_sphere_end);
    
    
	// get angle and rotation axis
	theta = glm::degrees(glm::acos(glm::dot(point_on_sphere_begin, point_on_sphere_end)));
	axis_of_rotation = glm::cross(point_on_sphere_end, point_on_sphere_begin);
    
	// rotate quatrion
	view_quat_new = glm::rotate(view_quat_old, theta, axis_of_rotation);
}

void VirtualTrackball::setWindowSize(int w, int h) {
	this->w = w;
	this->h = h;
}

glm::vec2 VirtualTrackball::getNormalizedWindowCoordinates(int x, int y) {
	glm::vec2 p;
	p[0] = x/static_cast<float>(w) - 0.5f;
	p[1] = 0.5f - y/static_cast<float>(h);
	return p;
}

glm::vec3 VirtualTrackball::getClosestPointOnUnitSphere(int x, int y) {
	glm::vec2 normalized_coords;
	glm::vec3 point_on_sphere;
	float r;

	normalized_coords = getNormalizedWindowCoordinates(x, y);
	r = glm::length(normalized_coords);

	if (r < 0.5) { //Ray hits unit sphere
		point_on_sphere[0] = 2*normalized_coords[0];
		point_on_sphere[1] = 2*normalized_coords[1];
		point_on_sphere[2] = sqrt(1 - 4*r*r);

		point_on_sphere = glm::normalize(point_on_sphere);
	}
	else { //Ray falls outside unit sphere
		point_on_sphere[0] = normalized_coords[0]/r;
		point_on_sphere[1] = normalized_coords[1]/r;
		point_on_sphere[2] = 0;            
	}

	return point_on_sphere;
}

glm::mat4 VirtualTrackball::getTransform() {
	return quatToMat4(view_quat_new);
}