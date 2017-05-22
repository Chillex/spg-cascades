#include "Window.h"
#include <iostream>
#include "Color.h"
#include "Input.h"

Window::Window(const GLuint width, const GLuint height, const GLchar* title)
	: m_width(width)
	, m_height(height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	// using the core profile results in invalid operation errors when calling legacy functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glfwWindowHint(GLFW_SAMPLES, 0);

	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if(m_window == nullptr)
	{
		std::cout << "ERROR::WINDOW: failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	else
	{
		glfwMakeContextCurrent(m_window);

		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		InitGlew();
	}
}

Window::~Window()
{
	glfwTerminate();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(m_window);
}

void Window::InitGlew()
{
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::WINDOW: failed to initialize GLEW" << std::endl;
		glfwTerminate();
	}
	else
	{
		int width;
		int height;
		glfwGetFramebufferSize(m_window, &width, &height);
		
		glViewport(0, 0, width, height);

		glEnable(GL_MULTISAMPLE);
	}
}

GLuint Window::GetWidth() const
{
	return m_width;
}

GLuint Window::GetHeight() const
{
	return m_height;
}

GLFWwindow* Window::GetGLFWWindow() const
{
	return m_window;
}

bool Window::ShouldClose() const
{
	return glfwWindowShouldClose(m_window) != 0;
}

void Window::SetClearColor(Color color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void Window::SetInput(Input* input)
{
	m_input = input;

	glfwSetWindowUserPointer(m_window, this);

	glfwSetKeyCallback(m_window, KeyCallback);
	glfwSetCursorPosCallback(m_window, MouseCallback);
	glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	Input* input = static_cast<Window*>(glfwGetWindowUserPointer(window))->m_input;
	input->HandleKey(key, action);
}

void Window::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	Input* input = static_cast<Window*>(glfwGetWindowUserPointer(window))->m_input;
	input->HandleCursor(xpos, ypos);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Input* input = static_cast<Window*>(glfwGetWindowUserPointer(window))->m_input;
	input->HandleMouseButton(button, action, mods);
}
