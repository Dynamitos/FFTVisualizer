#pragma once
#include "Utils/Resources.h"

class Display
{
public:
	Display();
	~Display();
	void showWindow();
	void createDisplay(AudioVisualizerInfo visualizerInfo);
	void updateDisplay();
	void closeDisplay();
	inline uint32_t getWidth() { return width; }
	inline uint32_t getHeight() { return height; }
	inline double getCurrentFrameTime() { return currentFrameDelta; }
	inline GLFWwindow* getWindow() { return window; }
	inline bool shouldClose() { return glfwWindowShouldClose(window); }
private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, double xPos, double yPos);

	GLFWwindow* window;
	uint32_t width = 1280;
	uint32_t height = 720;
	double lastFrameTime;
	double currentFrameDelta;
	int nbFrames;
};