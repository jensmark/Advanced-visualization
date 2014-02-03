#ifndef _VIRTUALTRACKBALL__
#define _VIRTUALTRACKBALL__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

/**
  * Simple class that implements a "virtual trackball"
  * 
  */
class VirtualTrackball {
public:
	VirtualTrackball();
	~VirtualTrackball();

	/**
	  * Called when we click the mouse on screen. Finds and
	  * sets rotate_begin_vec to be the vector from the origin
	  * to the closest point on the unit sphere.
	  */
	void rotateBegin(int x, int y);

	/**
	  * Resets the state to move the current camera quaternion
	  */
	void rotateEnd(int x, int y);

	/**
	  * Called when we move the mouse while clicking. Will move
	  * the camera using the "virtual trackball".
	  * Does nothing if we have not called rotateBegin first.
	  */
	void rotate(int x, int y);

	/**
	  * Returns the transformation matrix from the current quaternion
	  * @return the view matrix representing the rotation
	  */
	glm::mat4 getTransform();


	/**
	  * Sets the window size. This is important to be able to 
	  * make sure the virtual trackball fills the whole window
	  */
	void setWindowSize(int w, int h);

private:
	/**
	  * Returns the normalized (x=[-0.5, 0.5], y=[-0.5, 0.5]) window
	  * coordinates from absolute window coordinates (x=[0, w], y=[0, h]).
	  * Note that we flip the y-axis.
	  */
	glm::vec2 getNormalizedWindowCoordinates(int x, int y);

	/**
	  * Function that computes the closest 3D point on the unit sphere
	  * from the 2D window position.
	  */
	glm::vec3 getClosestPointOnUnitSphere(int x, int y);

	bool rotating; //Boolean to say if we should rotate or not
	unsigned int w; //Window width
	unsigned int h; //Window height

	glm::fquat view_quat_old; //View matrix that represents the old camera position
	glm::fquat view_quat_new; //View matrix that represents the new camera position

	glm::vec3 point_on_sphere_begin; //Vector from origin to first point on the unit sphere
};

#endif