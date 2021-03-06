#ifndef INPUT_H
#define INPUT_H

#include "Window.h"

class Input
{
public:
	Input();
	~Input();

	void HandleKey(int key, int action);
	void HandleCursor(double xpos, double ypos);
	void HandleMouseButton(int button, int action, int mods);
	void Update(void);

	bool IsKeyDown(int key) const;
	bool IsKeyPressed(int key) const;

	bool HasUnreadMouseChanges(void) const;
	void MouseChangesRead(void);
	GLfloat GetLastMouseX(void) const;
	GLfloat GetLastMouseY(void) const;
	GLfloat GetMouseOffsetX(void) const;
	GLfloat GetMouseOffsetY(void) const;
private:
	bool m_keys[1024] = { false }; // stores the pressed keys
	bool m_keys_prev[1024] = { false }; // stores the pressed keys from the last frame

	bool m_firstHandleCursor;
	bool m_mouseCoordsChanged;
	GLfloat m_lastMouseX;
	GLfloat m_lastMouseY;
	GLfloat m_mouseOffsetX;
	GLfloat m_mouseOffsetY;
};

#endif