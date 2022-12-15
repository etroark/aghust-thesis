#include "../include/camera.h"

void Camera::updateCameraVectors()
{
	_front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_front.y = sin(glm::radians(_pitch));
	_front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

	_front = glm::normalize(_front);
	_right = glm::normalize(glm::cross(_front, _worldUp));
	_up = glm::normalize(glm::cross(_right, _front));
}

Camera::Camera()
{
	updateCameraVectors();
}

const glm::mat4 Camera::getViewMatrix()
{
	updateCameraVectors();

	_ViewMatrix = glm::lookAt(_position, _position + _front, _up);

	return _ViewMatrix;
}

void Camera::move(const float& dt, const int direction)
{
	switch (direction)
	{
	case FORWARD:
		_position += _front * _movementSpeed * dt;
		break;
	case BACKWARD:
		_position -= _front * _movementSpeed * dt;
		break;
	case LEFT:
		_position -= _right * _movementSpeed * dt;
		break;
	case RIGHT:
		_position += _right * _movementSpeed * dt;
		break;
	default:
		break;
	}
}

void Camera::rotate(const float& dt, const double& offsetX, const double& offsetY)
{
	_pitch += static_cast<GLfloat>(offsetY) * _sensitivity * dt;
	_yaw += static_cast<GLfloat>(offsetX) * _sensitivity * dt;

	if (_pitch > 89.f)
		_pitch = 89.f;
	else if (_pitch < -89.f)
		_pitch = -89.f;

	if (_yaw > 360.f || _yaw < -360.f)
		_yaw = .0f;
}

void Camera::reset()
{
	_position = glm::vec3(0.f);

	_pitch = 0.f;
	_yaw = 0.f;
	_roll = 0.f;
}