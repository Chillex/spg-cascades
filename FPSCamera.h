#ifndef FPS_CAMERA_H
#define FPS_CAMERA_H

#include <glm/glm.hpp>
#include <GL/glew.h>

class FPSCamera
{
public:
	enum Movement
	{
		FORWARD = 0,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	FPSCamera(glm::vec3 position, glm::vec3 up, float fieldOfView, float speed, float sensitivity);
	~FPSCamera();

	void Move(Movement dir, float deltaTime);
	void HandleMouseMovement(GLfloat offsetX, GLfloat offsetY);

	void SetRotation(glm::quat rotation);

	glm::mat4 GetProjectionMatrix(GLfloat width, GLfloat height) const;
	glm::mat4 GetViewMatrix(void) const;
	glm::vec3 GetPosition(void) const;
	glm::quat GetRotation(void) const;

	void PrintInfo(void) const;

private: 
	float m_yaw;
	float m_pitch;
	float m_roll;

	float m_fov;

	float m_speed;
	float m_sensitivity;

	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraFront;
	glm::vec3 m_cameraUp;

	glm::vec3 m_cameraRight;
	glm::vec3 m_worldUp;

	void UpdateVectors(void);
};

#endif
