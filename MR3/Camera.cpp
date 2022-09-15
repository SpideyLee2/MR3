#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up) : m_Position(pos), m_Front(front), m_WorldUp(up) {
	m_Yaw = -90.0f;
	m_Pitch = 0.0f;

	m_MouseSensitivity = 0.1f;
	m_ScrollSensitivity = 1.5f;

	m_Speed = 5.0f;
	m_SpeedMultiplier = 3.0f;
	m_TempSpeed = m_Speed;

	m_Fov = 45.0f;

	m_PitchConstraintHi = 89.0f;
	m_PitchConstraintLo = -89.0f;
	m_FovConstraintHi = 90.0f;
	m_FovConstraintLo = 10.0f;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(m_Position, m_Position + m_Front, m_WorldUp);
}

void Camera::input(CAMERA_MOVEMENT movementDirection, double deltaTime) {
	float velocity = m_TempSpeed * static_cast<float>(deltaTime);
	if (movementDirection == CAMERA_MOVEMENT::Forward) {
		m_Position += m_Front * velocity;
	}
	if (movementDirection == CAMERA_MOVEMENT::Backward) {
		m_Position -= m_Front * velocity;
	}
	if (movementDirection == CAMERA_MOVEMENT::Left) {
		m_Position -= glm::normalize(glm::cross(m_Front, m_WorldUp)) * velocity;
	}
	if (movementDirection == CAMERA_MOVEMENT::Right) {
		m_Position += glm::normalize(glm::cross(m_Front, m_WorldUp)) * velocity;
	}
	if (movementDirection == CAMERA_MOVEMENT::Up) {
		m_Position += m_WorldUp * velocity;
	}
	if (movementDirection == CAMERA_MOVEMENT::Down) {
		m_Position -= m_WorldUp * velocity;
	}
}

void Camera::applySpeedMultiplier() {
	m_TempSpeed = m_Speed * m_SpeedMultiplier;
}

void Camera::removeSpeedMultiplier() {
	m_TempSpeed = m_Speed;
}

void Camera::processMouseMove(float xOffset, float yOffset) {
	m_Yaw += xOffset * m_MouseSensitivity;
	m_Pitch += yOffset * m_MouseSensitivity;

	if (m_Pitch > m_PitchConstraintHi)
		m_Pitch = m_PitchConstraintHi;
	else if (m_Pitch < m_PitchConstraintLo)
		m_Pitch = m_PitchConstraintLo;

	glm::vec3 direction;
	direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(-m_Pitch));
	direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(direction);
}

void Camera::processMouseScroll(float yOffset) {
	m_Fov -= yOffset * m_ScrollSensitivity;

	if (m_Fov > m_FovConstraintHi)
		m_Fov = m_FovConstraintHi;
	else if (m_Fov < m_FovConstraintLo)
		m_Fov = m_FovConstraintLo;
}

glm::mat4 Camera::myLookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 wUp) {
	glm::vec3 cDirection = glm::normalize(pos - target);
	glm::vec3 cRight = glm::normalize(glm::cross(wUp, cDirection));
	glm::vec3 cUp = glm::cross(cDirection, cRight);

	glm::mat4 rotation = glm::mat4(cRight.x, cUp.x, cDirection.x, 0.0f,
								   cRight.y, cUp.y, cDirection.y, 0.0f,
								   cRight.z, cUp.z, cDirection.z, 0.0f,
								   0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 translation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
									  0.0f, 1.0f, 0.0f, 0.0f,
									  0.0f, 0.0f, 1.0f, 0.0f,
									  -pos.x, -pos.y, -pos.z, 1.0f);

	return rotation * translation;
}