#pragma once

#include "libs.h"

/**
 * enum containing directions
 */
enum direction
{
	FORWARD = 0,
	BACKWARD,
	LEFT,
	RIGHT
};

/**
 * forward class declaration
 */
class Gui;

/**
  * camera class
  */
class Camera
{
	friend Gui;

	glm::mat4 _ViewMatrix = glm::mat4(.0f);	/**< matrix that can transform everything around the camera*/

	GLfloat _movementSpeed = 10.f; /**< camera movement speed*/
	GLfloat _sensitivity = 5.f;	/**< mouse sensitivity*/

	glm::vec3 _worldUp = glm::vec3(.0f, 1.f, .0f);	/**< world up vector with default parameters*/
	glm::vec3 _position = glm::vec3(.0f);	/**< camera position*/
	glm::vec3 _front;	/**< camera front vector (negative values for back)*/
	glm::vec3 _right;	/**< camera right vector (negative values for left)*/
	glm::vec3 _up;	/**< camera up vector (negative values for down)*/

	GLfloat _pitch = .0f;	/**< around the side-to-side axis*/
	GLfloat _yaw = .0f;	/**< around the vertical axis*/
	GLfloat _roll = .0f;	/**< around the front-to-back axis*/

	/**
	 * method that updates the camera vectors based on changed attributes
	 */
	void updateCameraVectors();

public:

	/**
	 * class constructor, updates camera vectors
	 */
	Camera();

	/**
	 * view matrix accessor; updates view matrix with glm::lookAt function and returns it
	 * @return view matrix const values
	 */
	const glm::mat4 getViewMatrix();
	/**
	 * camera position accessor
	 * @return camera position vector const values
	 */
	inline const glm::vec3 getPosition() const { return _position; }

	/**
	 * method for moving the camera based on direction caused by key input
	 * @param dt delta time
	 * @param direction requested direction
	 */
	void move(const float& dt, const int direction);
	/**
	 * method for rotating the camera based on mouse offset
	 * @param dt delta time
	 * @param offsetX mouse X offset
	 * @param offsetY mouse Y offset
	 */
	void rotate(const float& dt, const double& offsetX, const double& offsetY);
	
	/**
	 * method that resets the camera position, pitch, yaw and roll to the values of 0
	 */
	void reset();
};