#pragma once

#include <glm/glm.hpp>

enum class CAMERA_MOVEMENT {
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

class Camera {
public:
	glm::vec3 m_Position;
	glm::vec3 m_Front;

	float m_Yaw, m_Pitch;
	float m_MouseSensitivity, m_ScrollSensitivity;
	float m_Speed, m_SpeedMultiplier;

	float m_Fov;
	float m_PitchConstraintHi, m_PitchConstraintLo;
	float m_FovConstraintHi, m_FovConstraintLo;

	Camera(glm::vec3 pos, glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

	void input(CAMERA_MOVEMENT movementDirection, double deltaTime);
	void applySpeedMultiplier();
	void removeSpeedMultiplier();
	void processMouseMove(float xOffset, float yOffset);
	void processMouseScroll(float yOffset);
	glm::mat4 getViewMatrix();

private:
	glm::vec3 m_WorldUp;
	float m_TempSpeed;
	glm::mat4 myLookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
};
