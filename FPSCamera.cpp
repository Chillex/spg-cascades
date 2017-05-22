#include "FPSCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

FPSCamera::FPSCamera(glm::vec3 position, glm::vec3 up, float fieldOfView, float speed, float sensitivity)
	: m_pitch(0.0f)
	, m_yaw(245.0f)
	, m_roll(0.0f)
	, m_fov(fieldOfView)
	, m_speed(speed)
	, m_sensitivity(sensitivity)
	, m_cameraPos(position)
	, m_worldUp(up)
{
	UpdateVectors();
}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::Move(Movement dir, float deltaTime)
{
	float velocity = m_speed * deltaTime;

	if (dir == Movement::FORWARD)
		m_cameraPos += m_cameraFront * velocity;
	if (dir == Movement::BACKWARD)
		m_cameraPos -= m_cameraFront * velocity;
	if (dir == Movement::RIGHT)
		m_cameraPos += m_cameraRight * velocity;
	if (dir == Movement::LEFT)
		m_cameraPos -= m_cameraRight * velocity;
}

void FPSCamera::HandleMouseMovement(GLfloat offsetX, GLfloat offsetY)
{
	offsetX *= m_sensitivity;
	offsetY *= m_sensitivity;

	m_yaw += offsetX;
	m_pitch += offsetY;

	// restrain pitch to avoid gimble lock
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;

	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	UpdateVectors();
}

void FPSCamera::SetRotation(glm::quat rotation)
{
	glm::vec3 euler = glm::degrees(glm::eulerAngles(rotation));

	m_pitch = euler.x;
	m_yaw = euler.y;

	UpdateVectors();
}

glm::mat4 FPSCamera::GetProjectionMatrix(GLfloat width, GLfloat height) const
{
	return glm::perspective(m_fov, width / height, 0.1f, 100.0f);
}

glm::mat4 FPSCamera::GetViewMatrix(void) const
{
	return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
}

glm::vec3 FPSCamera::GetPosition(void) const
{
	return m_cameraPos;
}

glm::vec3 FPSCamera::GetDirection(void) const
{
	return m_cameraFront;
}

glm::quat FPSCamera::GetRotation(void) const
{
	glm::quat quatX = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat quatY = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat quatZ = glm::angleAxis(glm::radians(m_roll), glm::vec3(0.0f, 0.0f, 1.0f));

	return quatX * quatY * quatZ;
}

void FPSCamera::PrintInfo() const
{
	std::cout << m_cameraPos.x << "/" << m_cameraPos.y << "/" << m_cameraPos.z << std::endl;
	std::cout << m_yaw << std::endl;
}

void FPSCamera::UpdateVectors(void)
{
	//calculate front
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	newFront.y = sin(glm::radians(m_pitch));
	newFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_cameraFront = glm::normalize(newFront);

	// calculate right and up
	m_cameraRight = glm::normalize(glm::cross(m_worldUp, m_cameraFront));
	m_cameraUp = glm::normalize(glm::cross(m_cameraFront, m_cameraRight));
}
