#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
	Window(const int& screenWidth, const int& screenHeight, const char* title);
	~Window();

	//template<class A, class B, class T>
	//void setCallback(A (*pfnGlfwCallback)(GLFWwindow* window, void(*T)(...)), B (*pfn)(...));
	void setViewportSize(const int& x, const int& y, const int& width, const int& height);
	void setFramebufferSizeCallback(const GLFWframebuffersizefun& callback) const;
	void setCursorPosCallback(const GLFWcursorposfun& callback) const;
	void setScrollCallback(const GLFWscrollfun& callback) const;
	void setKeyCallback(const GLFWkeyfun& callback) const;
	void setInputMode(const int& mode, const int& value) const;
	void swapInterval(bool b);
	void swapBuffers() const;
	int getKey(const int& key) const;
	void setMonitor(GLFWmonitor* monitor, const int& xPos, const int& yPos, 
						  const int& width, const int& height, const int& refreshRate) const;

	bool getWindowShouldClose() const;
	void setWindowShouldClose(bool b);

private:
	GLFWwindow* m_pWindow;

	int m_WindowWidth = 800;
	int m_WindowHeight = 600;
	int m_ViewportWidth = 800;
	int m_ViewportHeight = 600;
	std::string m_Title;
};