#include "Display.h"

using namespace GL;

Display::Display()
{
}

Display::~Display()
{
}

void Display::showWindow()
{
	glfwShowWindow(window);
	lastFrameTime = (float)glfwGetTime();
}

void Display::createDisplay(AudioVisualizerInfo visualizerInfo)
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	WIDTH = visualizerInfo.screenDimensions.x;
	HEIGHT = visualizerInfo.screenDimensions.y;
	bool isFullscreen = false;
	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	if (vidMode->width == WIDTH && vidMode->height == HEIGHT)
	{
		isFullscreen = true;
	}
	window = glfwCreateWindow(WIDTH, HEIGHT, visualizerInfo.name, isFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, &keyCallback);
	glfwSetCursorPosCallback(window, &mouseCallback);
	glfwSwapInterval(visualizerInfo.vSync ? 1 : 0);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		exit(EXIT_FAILURE);
	}
}


void Display::updateDisplay()
{
	glfwPollEvents();
	double currentTime = glfwGetTime();
	currentFrameDelta = currentTime - lastFrameTime;
	glfwSwapBuffers(window);
}
void Display::closeDisplay()
{
	glfwTerminate();
	//system("PAUSE");
}
bool Display::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Display::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input::keys[key] = action != GLFW_RELEASE;
}

void Display::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
}

