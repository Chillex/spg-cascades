#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input;
struct Color;

class Window
{
public:
	Window(const GLuint width, const GLuint height, const GLchar* title);
	~Window();

	void InitGlew(void);

	GLuint GetWidth(void) const;
	GLuint GetHeight(void) const;
	GLFWwindow* GetGLFWWindow(void) const;

	void SwapBuffers(void);
	bool ShouldClose(void) const;
	void SetClearColor(Color color);
	void SetInput(Input* input);

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

	Input* m_input;
protected:
	GLuint m_width;
	GLuint m_height;
	GLFWwindow* m_window;
};

#endif