#pragma once
#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#include "Utils/Resources.h"

class SDLDisplay
{
public:
	SDLDisplay(int width, int height);
	~SDLDisplay();
	
	void drawFFT(float* fftOut, int count);
private:
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	int width, height;
	std::vector<SDL_Point> points;
};