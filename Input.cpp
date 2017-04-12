#include "Input.h"

#include <cstring>
#include <iostream>

Input::Input()
	: m_firstHandleCursor(true)
	, m_mouseCoordsChanged(false)
	, m_lastMouseX(0.0f)
	, m_lastMouseY(0.0f)
	, m_mouseOffsetX(0.0f)
	, m_mouseOffsetY(0.0f)
{
}

Input::~Input()
{
}

void Input::HandleKey(int key, int action)
{
	if(key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			m_keys[key] = true;
		if (action == GLFW_RELEASE)
			m_keys[key] = false;
	}
}

void Input::HandleCursor(double xpos, double ypos)
{
	if (m_firstHandleCursor)
	{
		m_lastMouseX = xpos;
		m_lastMouseY = ypos;
		m_firstHandleCursor = false;
	}

	m_mouseOffsetX = xpos - m_lastMouseX;
	m_mouseOffsetY = m_lastMouseY - ypos; // y-coords are bottom-top

	m_lastMouseX = xpos;
	m_lastMouseY = ypos;

	m_mouseCoordsChanged = true;
}

bool Input::IsKeyDown(int key) const
{
	if (key >= 0 && key < 1024)
		return m_keys[key];

	return false;
}

bool Input::HasUnreadMouseChanges() const
{
	return m_mouseCoordsChanged;
}

void Input::MouseChangesRead()
{
	m_mouseCoordsChanged = false;
}

GLfloat Input::GetLastMouseX() const
{
	return m_lastMouseX;
}

GLfloat Input::GetLastMouseY() const
{
	return m_lastMouseY;
}

GLfloat Input::GetMouseOffsetX() const
{
	return m_mouseOffsetX;
}

GLfloat Input::GetMouseOffsetY() const
{
	return m_mouseOffsetY;
}
