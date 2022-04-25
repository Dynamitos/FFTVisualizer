#pragma once
#include <SDL.h>
#include <vector>
#include <iostream>
#include "Utils/Resources.h"
#include "AudioVisualizer.h"

class SDLDisplay : public AudioVisualizer
{
public:
	SDLDisplay();
	~SDLDisplay();
	
	virtual void init(AudioVisualizerInfo visualizerInfo) override;
	virtual void renderData(std::unique_ptr<SampleContainer> data) override;
	virtual bool shouldClose();
private:
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	int width, height;
	std::vector<SDL_Point> points;
};