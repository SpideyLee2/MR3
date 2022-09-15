#include "Application.h"

int main() {
	MR3::Application* app = MR3::Application::start();
	app->run();
	app->shutdown();
}