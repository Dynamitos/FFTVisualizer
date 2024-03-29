#include "Display.h"

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	width = visualizerInfo.screenDimensions.x;
	height = visualizerInfo.screenDimensions.y;
	bool isFullscreen = false;
	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	if (vidMode->width == width && vidMode->height == height)
	{
		isFullscreen = true;
	}
	window = glfwCreateWindow(width, height, visualizerInfo.name, isFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	glfwSetKeyCallback(window, &keyCallback);
	glfwSetCursorPosCallback(window, &mouseCallback);
	glfwSwapInterval(visualizerInfo.vSync ? 1 : 0);
	glfwMakeContextCurrent(window);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
}


void Display::updateDisplay()
{
	glfwPollEvents();
	double currentTime = glfwGetTime();
	currentFrameDelta = currentTime - lastFrameTime;
	lastFrameTime = currentTime;
	glfwSwapBuffers(window);
}

void Display::closeDisplay()
{
	glfwTerminate();
	//system("PAUSE");
}

void Display::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input::keys[key] = action != GLFW_RELEASE;
}

void Display::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
}

