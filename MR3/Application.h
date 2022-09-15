#pragma once

#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include <glm/glm.hpp>

#include "Shader.h"

namespace MR3 {

	struct AppSettings {
		unsigned int width = 800;
		unsigned int height = 600;
		const char* title = "MR3 Renderer";
	};

	class Application {
	public:
		static Application* start(AppSettings&& appInit = AppSettings());

		Application(const Application& app) = delete;
		Application(Application&& app) = delete;
		Application& operator=(const Application& app) = delete;
		Application& operator=(Application&& app) = delete;

		void init();
		void run();
		void shutdown();
		void processInput();

		double m_DeltaTime = 0.0f;
		int m_SizeOfMat4 = sizeof(glm::mat4);
		std::unordered_map<std::string, Shader, std::hash<std::string>> m_Shaders;

	private:
		Application(const AppSettings& appInit);
		~Application();

		//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		double m_LastFrameTime = 0.0f;
		double m_CurrFrameTime = 0.0f;

		AppSettings m_AppSettings;
		GLFWwindow* m_pWindowHandle;
		ImGuiIO* imguiIO;
	};

}