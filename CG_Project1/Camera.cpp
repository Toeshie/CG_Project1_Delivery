#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	MovementSpeed(2.5f),
	MouseSensitivity(0.09f),
	Zoom(60.0f),
	isYLocked(false),
	lockedY(position.y)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	glm::vec3 movement(0.0f);
	
	if (isYLocked) {
		// Create flattened vectors for Y-locked movement
		glm::vec3 flatFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
		glm::vec3 flatRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));
		
		if (direction == FORWARD)
			movement += flatFront;
		if (direction == BACKWARD)
			movement -= flatFront;
		if (direction == LEFT)
			movement -= flatRight;
		if (direction == RIGHT)
			movement += flatRight;
	} else {
		// Normal movement when not Y-locked
		if (direction == FORWARD)
			movement += Front;
		if (direction == BACKWARD)
			movement -= Front;
		if (direction == LEFT)
			movement -= Right;
		if (direction == RIGHT)
			movement += Right;
	}

	// Normalize movement vector if moving diagonally
	if (glm::length(movement) > 0.0f) {
		movement = glm::normalize(movement);
		
		Position += movement * velocity;
		
		if (isYLocked) {
			Position.y = lockedY;  // Maintain the locked Y position
		}
	}
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Constrain the pitch angle to prevent screen flipping
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update the camera's vectors
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	float sensitivity = 2.0f; // Adjust for desired scroll sensitivity
	Zoom -= yoffset * sensitivity;

	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);

	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::toggleYLock() {
	isYLocked = !isYLocked;
	if (isYLocked) {
		lockedY = Position.y;  // Store current Y position
	}
}