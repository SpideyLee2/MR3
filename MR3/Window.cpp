#include "Window.h"

Window::Window(const int& width, const int& height, const char* title) 
	: m_WindowWidth(width), m_WindowHeight(height), m_Title(title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_pWindow = glfwCreateWindow(m_WindowWidth, m_WindowHeight, title, NULL, NULL);

	if (!m_pWindow) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(m_pWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to load GLAD" << std::endl;
		glfwTerminate();
		exit(-1);
	}
}

Window::~Window() {
	m_pWindow = nullptr;
}

// I don't know enough about template functions and function pointers to get this to work :P
//template<class A, class B, typename T>
//void Window::setCallback(A (*pfnGlfwCallback)(GLFWwindow* window, void(*T)(...)), B (*pfn)(...)) {
//	pfnGlfwCallback(m_pWindow, pfn);
//}

void Window::setViewportSize(const int& x, const int& y, const int& width, const int& height) {
	m_ViewportWidth = width;
	m_ViewportHeight = height;
	glViewport(x, y, width, height);
}

void Window::setFramebufferSizeCallback(const GLFWframebuffersizefun& callback) const {
	glfwSetFramebufferSizeCallback(m_pWindow, callback);
}

void Window::setCursorPosCallback(const GLFWcursorposfun& callback) const {
	glfwSetCursorPosCallback(m_pWindow, callback);
}

void Window::setScrollCallback(const GLFWscrollfun& callback) const {
	glfwSetScrollCallback(m_pWindow, callback);
}

void Window::setKeyCallback(const GLFWkeyfun& callback) const {
	glfwSetKeyCallback(m_pWindow, callback);
}

void Window::setInputMode(const int& mode, const int& value) const {
	glfwSetInputMode(m_pWindow, mode, value);
}

void Window::swapInterval(bool b) {
	glfwSwapInterval(b);
}

void Window::swapBuffers() const {
	glfwSwapBuffers(m_pWindow);
}

int Window::getKey(const int& key) const {
	return glfwGetKey(m_pWindow, key);
}

void Window::setMonitor(GLFWmonitor* monitor, const int& xPos, const int& yPos, const int& width, const int& height, const int& refreshRate) const {
	glfwSetWindowMonitor(m_pWindow, monitor, xPos, yPos, width, height, refreshRate);
}

bool Window::getWindowShouldClose() const {
	return glfwWindowShouldClose(m_pWindow);
}

void Window::setWindowShouldClose(bool b) {
	glfwSetWindowShouldClose(m_pWindow, b);
}